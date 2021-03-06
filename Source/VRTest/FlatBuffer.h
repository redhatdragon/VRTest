#pragma once

//Taken from https://github.com/redhatdragon/EchoEngine/tree/main/include
//Aside the helper BitBool lib this is under the unlicense

#include <stdint.h>
#include <vector>
#include <string>
#include <bitset>
#include "BitBool.h"

#define FB32_BIT_SET(number, n) number |= 1U << n;
#define FB32_BIT_CLEAR(number, n) number &= ~(1U << n);
#define FB32_BIT_TOGGLE(number, n) number ^= 1U << n;
#define FB32_BIT_TEST(number, n) (number >> n) & 1U;



//#define _FB_USE_RAWBYTE
//#define _FB_USE_BITSET
#define _FB_USE_BITBOOL //TODO: is having problems with large buffers for some reason?



template <typename T, uint32_t maxLength>
class FlatFlaggedBuffer {
	T data[maxLength];
	#ifdef _FB_USE_RAWBYTE
	uint8_t isValid[maxLength + 8];
	#elif defined _FB_USE_BITSET
	std::bitset<maxLength> isValid;
	#elif defined _FB_USE_BITBOOL
	BitBool<maxLength + 8> isValid;
	#endif
	uint32_t count = 0;
	uint32_t maxIndex = 0;
public:
	FlatFlaggedBuffer() {
		clear();
	}
	__forceinline void clear() {
		//isValid = std::bitset<maxLength>();
		for (uint32_t i = 0; i < maxLength; i++) {
			isValid[i] = 0;  //TODO: Required for BitBool, should have something more optimized.
		}
	}
	// Consider using in conjunction with incrementCount.
	__forceinline uint32_t toggleFirstInvalid() {
		#ifdef _FB_USE_RAWBYTE
		//NOTE: Used with normal byte buffer, byte per element not bit per element.
		uint32_t max = maxLength / 4;
		uint32_t* bufferAs32 = (uint32_t*)&isValid[0];
		for (unsigned int i = 0; i < max; i++) {
			if (bufferAs32[i] == -1) continue;
			for (unsigned int j = 0; j < 4; j++) {
				if (isValid[i * 4 + j] == 0) {
					setValid(i * 4 + j);
					return i * 4 + j;
				}
			}
		}

		#elif defined _FB_USE_BITSET
		//NOTE: Used with bitset
		for (unsigned int i = 0; i < maxLength; i++) {
			if (isValid[i] == 0) {
				setValid(i);
				return i;
			}
		}

		#elif defined _FB_USE_BITBOOL
		uint32_t max = (maxLength / (4 * 8))+(4*8);
		uint32_t* bufferAs32 = (uint32_t*)&isValid.data[0];
		for (unsigned int i = 0; i < max; i++) {
			if (bufferAs32[i] == -1) continue;
			for (unsigned int j = i*(4*8); j < i*(4*8)+4*8; j++) {
				if (isValid[j] == 0) {
					setValid(j);
					return j;
				}
			}
		}
		#endif

		return -1;
	}
	__forceinline void setInvalid(uint32_t index) {
		isValid[index] = 0;
	}
	__forceinline void setValid(uint32_t index) {
		isValid[index] = 1;
	}
	__forceinline uint32_t insert(const T& component) {
		uint32_t index = toggleFirstInvalid();
		count++;
		data[index] = component;
		return index;
	}
	__forceinline bool getIsValid(uint32_t index) {
		return (bool)isValid[index];
	}
	__forceinline uint32_t getCount() {
		return count;
	}
	// To be used with toggleFirstInvalid.
	__forceinline void incrementCount() {
		count++;
	}
	__forceinline void decrementCount() {
		count--;
	}
	__forceinline T& operator[](uint32_t index) {
		return data[index];
	}
};



template <typename T, uint32_t maxLength>
class FlatBuffer {
	T data[maxLength];
public:
	uint32_t count = 0;
	__forceinline T& operator[](uint32_t index) {
		return data[index];
	}
	__forceinline void push(T _data) {
		data[count] = _data;
		count++;
	}
	__forceinline void pop() {
		count--;
	}
	__forceinline void clear() {
		count = 0;
	}
};







#include <iostream>

#ifdef TESTING
static std::string T_FlatFlaggedBufferInitialize() {
	FlatFlaggedBuffer<uint32_t, 10> ffb;
	for (uint32_t i = 10; i > 0; i--) {
		ffb.insert(i);
	}
	if (ffb.getCount() != 10) {
		std::string retValue = "Count is incorrect, expected 10, instead got ";
		retValue += std::to_string(ffb.getCount());
		return retValue;
	}
	uint32_t expectedBuffer[10] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
	bool mismatch = false;
	for (uint32_t i = 0; i < ffb.getCount(); i++) {
		if (ffb[i] != expectedBuffer[i])
			mismatch = true;
	}
	if (mismatch) {
		std::string retValue = "Mismatched buffer, got...\n";
		for (uint32_t i = 0; i < ffb.getCount(); i++)
			retValue += std::to_string(ffb[i]) + std::string(" ");
		retValue += " \nexpected...\n";
		for (uint32_t i = 0; i < ffb.getCount(); i++)
			retValue += std::to_string(expectedBuffer[i]) + std::string(" ");;
		return retValue;
	}
	return "";
}
static std::string T_FlatFlaggedBufferBigInitialize() {
	constexpr uint32_t max = 100000;
	static FlatFlaggedBuffer<uint32_t, max> ffb;
	for (uint32_t i = 0; i < max; i++) {
		ffb.insert(i);
	}
	for (uint32_t i = 0; i < max; i++) {
		if (ffb[i] != i) {
			std::string retValue = "";
			retValue += "Failed to do a big initialization, error at index ";
			retValue += std::to_string(i);
			retValue += " expected index value but got ";
			retValue += std::to_string(ffb[i]);
			return retValue;
		}
	}
	return "";
}
#endif
static void testFlatFlaggedBuffer() {
#ifdef TESTING
	std::string dbgStr = "";
	dbgStr = T_FlatFlaggedBufferInitialize();
	if (dbgStr != "") {
		std::cout << dbgStr << std::endl;
		dbgStr = "";
	}
	dbgStr = T_FlatFlaggedBufferBigInitialize();
	if (dbgStr != "") {
		std::cout << dbgStr << std::endl;
		dbgStr = "";
	}
#endif
}