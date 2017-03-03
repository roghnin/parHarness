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



#ifndef SGL_MAP
#define SGL_MAP

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <queue>
#include <string>
#include <atomic>
#include "RMap.hpp"

class SGLMap : public RMap{

private:
	void lockAcquire(int32_t tid);
	void lockRelease(int32_t tid);

	std::map<int32_t, int32_t>* m=NULL;
	std::atomic<int32_t> lk;


public:
	SGLMap();
	~SGLMap();
	//SGLMap(std::map<int32_t, int32_t>* contents);

	bool map(int32_t key, int32_t val, int tid);
	int32_t unmap(int32_t key, int tid);
	int32_t get(int32_t key, int tid);
	
};

class SGLMapFactory : public RMapFactory{
	SGLMap* build(GlobalTestConfig* gtc){
		return new SGLMap();
	}
};

#endif
