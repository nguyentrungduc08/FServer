/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Hasing.h
 * Author: hydra
 *
 * Created on March 24, 2018, 1:23 AM
 */

#ifndef HASING_H
#define HASING_H

#include "fileserver.h"

const int BUCKETS = 1e6 + 3;

template <class Type>
struct HashTable {
    std::vector< std::pair<Type,Type> > h[P];

public:
    void insert(Type key, Type value) {
        int hkey = getHash(key);
        for (auto p : h[hkey]) {
            if (p.first == key) {
                
                return;
            }
        }

        h[hkey].emplace_back(key, value);
    }

    int find(Type key) {
        int hkey = getHash(key);
        for(auto p : h[hkey]) {
            if (p.first == key) {
                
                return p.value;
            }
        }
        
        return 0;
    }

private:
    int getHash(Type key) {
        
        return key % P;
    }
};

#endif /* HASING_H */

