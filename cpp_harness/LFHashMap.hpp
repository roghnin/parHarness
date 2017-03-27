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



#ifndef LFHASH_MAP
#define LFHASH_MAP

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <queue>
#include <string>
#include <atomic>
#include "RMap.hpp"

#define ARRLEN 64 //make it fixed for now.
#define CAS_WEAK 0
#define CAS_STRONG 1
#define UNMAP_TRAVERSE 0
#define UNMAP_GET 1

class LFHMNode {
public:	//a dirty way...
	int32_t key;
	int32_t val;
	LFHMNode *next;
	LFHMNode(int32_t k, int32_t v): key(k), val(v){next = NULL;}
	//LFHMNode(const LFHMNode&):key(src.key), val(src.val), next(src.next){}
	~LFHMNode();
};

class LFHashMap : public RMap{

private:
	//void lockAcquire(int32_t tid);
	//void lockRelease(int32_t tid);
	std::atomic<LFHMNode*>* arr=NULL; //TODO: double-check the type here.
	GlobalTestConfig* gtc = NULL;
	int cas_weak_strong = CAS_WEAK;
	int unmap_traverse_get = UNMAP_TRAVERSE;
	//std::atomic<int32_t> lk;
	int hash(int32_t key) {return key%ARRLEN;}
	int32_t traverse(LFHMNode *head, int32_t key);

public:
	LFHashMap(GlobalTestConfig* gtc);
	~LFHashMap();
	//LFHashMap(std::map<int32_t, int32_t>* contents);

	bool map(int32_t key, int32_t val, int tid);
	int32_t unmap(int32_t key, int tid);
	int32_t get(int32_t key, int tid);
	
};

class LFHashMapFactory : public RMapFactory{
	LFHashMap* build(GlobalTestConfig* gtc){
		return new LFHashMap(gtc);
	}
};

#endif
