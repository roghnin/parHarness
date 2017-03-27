/*

Copyright 2015 University of Rochester

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. 

*/



#include "LFHashMap.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

LFHashMap::LFHashMap(GlobalTestConfig* Gtc){
	gtc = Gtc;
	if (gtc->environment["CAS"] == "STRONG"){
		std::cout<<"CAS = STRONG!"<<'\n';
		cas_weak_strong = CAS_STRONG;
	}
	if (gtc->environment["UNMAP"] == "GET"){
		unmap_traverse_get = UNMAP_GET;
		std::cout<<"UNMAP = GET!"<<'\n';
	}
	arr = new std::atomic<LFHMNode*>[ARRLEN];
	for (int i = 0; i < ARRLEN; ++i)
	{
		arr[i].store(NULL);
	}
}

LFHashMap::~LFHashMap(){}

int32_t LFHashMap::traverse(LFHMNode* head, int32_t key){
	LFHMNode* p = head;
	//assert ((unsigned long)p != (unsigned long)0x1);
	while(p != NULL){
		if (p->key == key){
			return p->val;
		}
		p = p->next;
	}
	return NULL;
}

int32_t LFHashMap::get(int32_t key, int tid){
	std::atomic<LFHMNode*>* bucket = &arr[hash(key)]; 
	return traverse(bucket->load(std::memory_order::memory_order_acquire), key);
	//LFHMNode* p = bucket->load(std::memory_order::memory_order_acquire);
	//while(p){ //moved to LFHashMap::traverse()
	//	if (p->key == key){
	//		return p->val;
	//	}
	//	p = p->next;
	//}
	//return NULL;
}

bool LFHashMap::map(int32_t key, int32_t val, int tid){
	std::atomic<LFHMNode*>* bucket = &arr[hash(key)];
	LFHMNode* head;
	LFHMNode* newnd = new LFHMNode(key, val);
	int32_t check;
	assert (newnd != NULL);
	head = bucket->load(std::memory_order::memory_order_acquire); //(*)
	if (cas_weak_strong = CAS_WEAK){
		do{
			//here, no need to do (*) again, since CAS of C++ will do it automatically when fails.
			newnd->next = head;
		} while ((bucket->compare_exchange_weak(head, newnd, std::memory_order::memory_order_acq_rel, std::memory_order_acquire)) == false);
	} else {
		do{
			newnd->next = head;
		} while ((bucket->compare_exchange_strong(head, newnd, std::memory_order::memory_order_acq_rel, std::memory_order_acquire)) == false);
	}
	return true;
}

int32_t LFHashMap::unmap(int32_t key, int tid){
	int32_t ret = NULL;
	std::atomic<LFHMNode*>* bucket = &arr[hash(key)];
	LFHMNode* head;
	LFHMNode* newnd = new LFHMNode(key, NULL);
	assert (newnd != NULL);
	head = bucket->load(std::memory_order::memory_order_acquire); //(*)
	
	//TODO: this is soooooo silly.
	if (cas_weak_strong == CAS_WEAK && unmap_traverse_get == UNMAP_TRAVERSE){	
		do{
			newnd->next = head;
			ret = traverse (head, key);
		} while ((bucket->compare_exchange_weak(head, newnd, std::memory_order::memory_order_acq_rel, std::memory_order_acquire)) == false);
	} else if (cas_weak_strong == CAS_WEAK && unmap_traverse_get == UNMAP_GET) {
		do{
			newnd->next = head;
			ret = get (key, tid);
		} while ((bucket->compare_exchange_weak(head, newnd, std::memory_order::memory_order_acq_rel, std::memory_order_acquire)) == false);
	} else if (cas_weak_strong == CAS_STRONG && unmap_traverse_get == UNMAP_TRAVERSE) {
		do{
			newnd->next = head;
			ret = traverse (head, key);
		} while ((bucket->compare_exchange_strong(head, newnd, std::memory_order::memory_order_acq_rel, std::memory_order_acquire)) == false);
	} else if (cas_weak_strong == CAS_STRONG && unmap_traverse_get == UNMAP_GET) {
		do{
			newnd->next = head;
			ret = get (key, tid);
		} while ((bucket->compare_exchange_strong(head, newnd, std::memory_order::memory_order_acq_rel, std::memory_order_acquire)) == false);
	}
	return ret;
}

