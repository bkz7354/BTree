#pragma once

#include <array>
#include <cassert>
#include <iostream>
#include <string>

template<typename T, size_t M = 100>
class B_tree{
private:
    static const int max_key = 2*M-1;
    static const int min_key = M-1;
    static const int max_child = 2*M;

    struct node{
        size_t fill = 0;
        std::array<node*, max_child> c;
        std::array<T, max_key> key;
        bool leaf;

        node(bool leaf_, size_t fill_): leaf{leaf_}, fill{fill_} {}

        size_t find_key(const T& val){
            size_t i = 0;
            while(i < fill && key[i] < val)
                ++i;
            
            return i;
        }

        T& get_max(){
            node* nd = this;
            while(!nd->leaf)
                nd = nd->c[nd->fill];
            
            return nd->key[nd->fill - 1];
        }



        void insert(const T& val);
        void split_child(size_t id);
        
        void remove(const T& val);
        void leaf_remove(size_t id);
        void inner_remove(size_t id);
        void remove_max();
        
        void balance(size_t id);
        void merge_children(size_t id); // merge c[id] with c[id + 1]
        void rotate_cw(size_t id);
        void rotate_ccw(size_t id);

        bool find(const T& val);
        
        void print(std::ostream& os, std::string pref);

        void clear_subtree();
    };
    
    node* m_root = nullptr;

    void root_insert(const T& val);
    void root_remove(const T& val);

public:
    void insert(const T& val){
        root_insert(val);
    }

    bool find(const T& val){
        if(m_root)
            return m_root->find(val);
        return false;
    }

    void print_tree(std::ostream& os){
        std::cout << "printing tree:" << std::endl;
        if(m_root)
            m_root->print(os, "");
        else
            os << "tree is empty" << std::endl;

        std::cout << std::endl;            
    }

    void remove(const T& val){
        if(m_root)
            root_remove(val);
    }

    ~B_tree(){
        if(m_root){
            m_root->clear_subtree();
            delete m_root;
            m_root = nullptr;
        }

    }
};


template<typename T, size_t M>
void B_tree<T, M>::root_insert(const T& val){
    if(!m_root){
        m_root = new node(true, 1);
        m_root->key[0] = val;
        return;
    }


    if(m_root->fill == max_key){
        node* new_root = new node(false, 0);
        new_root->c[0] = m_root;
        m_root = new_root;

        m_root->split_child(0);
    }

    m_root->insert(val);
}

template<typename T, size_t M>
void B_tree<T, M>::node::insert(const T& val){
    size_t pos = find_key(val);
    
    if(leaf){
        //to avoid duplicate keys
        if(pos < fill && key[pos] == val)
            return ;

        for(size_t i = fill; i > pos; --i)
            key[i] = std::move(key[i-1]);
        
        key[pos] = val;
        ++fill;
    }
    else{
        if(c[pos]->fill == max_key){
            this->split_child(pos);
            if(val > key[pos])
                ++pos;
        }
        //to avoid duplicate keys
        if(pos < fill && key[pos] == val)
            return ;

        c[pos]->insert(val);
    }
}


template<typename T, size_t M>
void B_tree<T, M>::node::split_child(size_t id){
    node* l = c[id];
    node* r = new node(l->leaf, M-1);
    
    for(size_t i = 0; i < M-1; ++i)
        r->key[i] = std::move(l->key[M + i]);
        
    if(!l->leaf)
        for(size_t i = 0; i < M; ++i)
            r->c[i] = l->c[M + i];
    l->fill = M-1;

    for(size_t i = fill; i > id; --i){
        c[i+1] = c[i];
        key[i] = std::move(key[i-1]);
    }
    c[id+1] = r;
    key[id] = std::move(l->key[M-1]);
    ++fill;
}

template<typename T, size_t M>
void B_tree<T, M>::root_remove(const T& val){
    m_root->remove(val);

    if(m_root->fill == 0){
        if(m_root->leaf)
            m_root = nullptr;
        else{
            node* new_root = m_root->c[0];
            delete m_root;
            m_root = new_root;
        }
    }

}

template<typename T, size_t M>
void B_tree<T, M>::node::remove(const T& val){
    size_t pos = find_key(val);

    if(pos < fill && key[pos] == val){
        if(leaf)
            leaf_remove(pos);
        else
            inner_remove(pos);
    }
    else{
        if(leaf)
            return ;

        c[pos]->remove(val);
        balance(pos);
    }
}

template<typename T, size_t M>
void B_tree<T, M>::node::leaf_remove(size_t id){
    --fill;
    for(size_t i = id; i < fill; ++i)
        key[i] = std::move(key[i+1]);
}

template<typename T, size_t M>
void B_tree<T, M>::node::inner_remove(size_t id){
    key[id] = std::move(c[id]->get_max());
    c[id]->remove_max();
    balance(id);
}

template<typename T, size_t M>
void B_tree<T, M>::node::remove_max(){
    if(leaf)
        leaf_remove(fill-1);
    else{
        c[fill]->remove_max();
        balance(fill);
    }
}

template<typename T, size_t M>
void B_tree<T, M>::node::balance(size_t id){
    if(c[id]->fill >= min_key)
        return ;
    
    if(id > 0 && c[id-1]->fill > min_key)
        rotate_cw(id-1);
    else if(id < fill && c[id+1]->fill > min_key)
        rotate_ccw(id);
    else if(id > 0)
        merge_children(id-1);
    else
        merge_children(id);
}

template<typename T, size_t M>
void B_tree<T, M>::node::rotate_cw(size_t id){
    node* l = c[id];
    node* r = c[id+1];

    for(size_t i = r->fill; i > 0; --i){
        r->key[i] = std::move(r->key[i-1]);
        r->c[i+1] = r->c[i];
    }
    r->c[1] = r->c[0];

    r->key[0] = std::move(key[id]);
    key[id] = std::move(l->key[l->fill-1]);

    r->c[0] = l->c[l->fill];

    --l->fill;
    ++r->fill;
}

template<typename T, size_t M>
void B_tree<T, M>::node::rotate_ccw(size_t id){
    node* l = c[id];
    node* r = c[id+1];

    l->key[l->fill] = std::move(key[id]);
    key[id] = std::move(r->key[0]);

    ++l->fill;
    --r->fill;

    l->c[l->fill] = r->c[0];

    for(size_t i = 0; i < r->fill; ++i){
        r->key[i] = std::move(r->key[i+1]);
        r->c[i] = r->c[i+1];
    }
    r->c[r->fill] = r->c[r->fill+1];
}
    
template<typename T, size_t M>
void B_tree<T, M>::node::merge_children(size_t id){
    node* l = c[id];
    node* r = c[id+1];

    l->key[l->fill] = std::move(key[id]);
    for(size_t i = 0; i < r->fill; ++i){
        l->key[l->fill + 1 + i] = std::move(r->key[i]);
        l->c[l->fill + 1 + i] = r->c[i];
    }
    l->c[l->fill + 1 + r->fill] = r->c[r->fill];
    l->fill = l->fill + r->fill + 1;
    delete r;

    --fill;
    for(size_t i = id; i < fill; ++i){
        key[i] = std::move(key[i+1]);
        c[i+1] = c[i+2];
    }
}



template<typename T, size_t M>
bool B_tree<T, M>::node::find(const T& val){
    size_t i = 0;
    for(; i < fill && key[i] <= val; ++i)
        if(key[i] == val)
            return true;

    if(!leaf)
        return c[i]->find(val);
    return false;
}


template<typename T, size_t M>
void B_tree<T, M>::node::print(std::ostream& os, std::string pref){
    os << pref << (leaf ? "x " : "> ");
    for(size_t i = 0; i < fill; ++i)
        os << key[i] << " ";
    os << std::endl;

    if(leaf)
        return ;

    std::string d_pref = "|   ";

    os << pref;
    for(size_t i = 0; i <= fill; ++i){
        if(i < fill)
            os << "+---";
        else
            os << "+";
        
        pref += d_pref;
    }
    os << std::endl;
    os << pref  << std::endl << pref << std::endl;
    for(size_t i = fill; i != size_t(-1); --i){
        pref = pref.substr(0, pref.size() - d_pref.size());
        c[i]->print(os, pref);
        os << pref << std::endl;
    }
}

template<typename T, size_t M>
void B_tree<T, M>::node::clear_subtree(){
    if(!leaf){
        for(size_t i = 0; i < fill; ++i){
            c[i]->clear_subtree();
            delete c[i];
            c[i] = nullptr;
        }
    }
}