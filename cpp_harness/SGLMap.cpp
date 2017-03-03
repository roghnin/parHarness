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



#include "SGLMap.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

SGLMap::SGLMap(){
	m = new std::map<int32_t, int32_t>();//probable optional alloc function available.
	lk.store(-1,std::memory_order::memory_order_release);
}

SGLMap::~SGLMap(){}

bool SGLMap::map(int32_t key, int32_t val, int tid){
	bool ret = true;
	std::map<int32_t, int32_t>::iterator it;
	lockAcquire(tid);
	if ((it = m->find(key)) == m->end()){//key not yet mapped.
		ret = m->insert(std::make_pair(key, val)).second;//might fail
	} else {
		it->second = val;
	}
	lockRelease(tid);
	return ret;
}

int32_t SGLMap::unmap(int32_t key, int tid){
	int32_t ret = 0;
	std::map<int32_t, int32_t>::iterator it;
	lockAcquire(tid);
	if ((it = m->find(key)) != m->end()){//key mapped
		ret = it->second;
		m->erase(it);
	}
	lockRelease(tid);
	return ret;
}

int32_t SGLMap::get(int32_t key, int tid){
	int32_t ret;
	lockAcquire(tid);
	ret = m->find(key)->second;
	lockRelease(tid);
	return ret;
}


// Simple test and set lock
/// There are better ways to do this...
void SGLMap::lockAcquire(int32_t tid){
	int unlk = -1;
	//while(!lk.compare_exchange_strong(unlk, tid, std::memory_order::memory_order_acq_rel)){//strong?
	while(!lk.compare_exchange_weak(unlk, tid, std::memory_order::memory_order_acq_rel)){

		unlk = -1; // compare_exchange puts the old value into unlk, so set it back
	}
	assert(lk.load()==tid);
}

void SGLMap::lockRelease(int32_t tid){
	assert(lk==tid);
	int unlk = -1;
	lk.store(unlk,std::memory_order::memory_order_release);
}

/*SGLMap::SGLMap(std::map<int32_t, int32_t>* contents){
	lk.store(-1,std::memory_order::memory_order_release);
	m = new std::map<int32_t, int32_t>(contents);//TODO: need test.
	//q->assign(contents->begin(),contents->end());
}*/

