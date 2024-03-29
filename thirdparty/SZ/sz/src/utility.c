/**
 *  @file utility.c
 *  @author Sheng Di, Sihuan Li
 *  @date Aug, 2018
 *  @brief 
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utility.h"
#include "sz.h"
#include "callZlib.h"
#include "zstd.h"

int compare_struct(const void* obj1, const void* obj2){
	struct sort_ast_particle * srt1 = (struct sort_ast_particle*)obj1;
	struct sort_ast_particle * srt2 = (struct sort_ast_particle*)obj2;
	return srt1->id - srt2->id;
}

void reorder_vars(SZ_VarSet* vset){
	SZ_Variable* v[7];
	SZ_Variable* v_tmp;
	int i, j;
	//v[0]
	for (v_tmp = vset->header->next, i = 0; i < 7; i++){
		v[i] = v_tmp;
		v_tmp = v_tmp->next;
	}
	//printf("here");
	size_t dataLen = computeDataLength(v[0]->r5, v[0]->r4, v[0]->r3, v[0]->r2, v[0]->r1);
	//sihuan debug
	//printf("the data length is (in sorting): %u", dataLen);
	struct sort_ast_particle* particle = (struct sort_ast_particle*) malloc(sizeof(struct sort_ast_particle)*dataLen);

	for (i = 0; i < dataLen; i++){
		particle[i].id = ((int64_t*)v[6]->data)[i];
	//	printf("%llu ", particle[i].id);
		for (j = 0; j < 6; j++)
			particle[i].var[j] = ((float*)v[j]->data)[i];
	}

	//sihuan debug
	#if 0
	printf("index before sorting: \n");
	for (i = 0; i < 5; i++){
		printf("%llu  ", particle[i].id);
		printf("%.5f  ", ((float*)v[0]->data)[i]);
	}
	#endif
	//printf("\n");
	//sihuan debug
	//for (i = 0; i < 5; i++)//{
		//for (j = 0; j < 6; j++)
		//	printf("%.5f  ", particle[i].var[j]);
	//		printf("%llu  ", particle[i].id );
	///}
	//printf("\n\n");


	qsort(particle, dataLen, sizeof(struct sort_ast_particle), compare_struct);
	for (i = 0; i < dataLen; i++){
		((int64_t*)v[6]->data)[i] = particle[i].id;
		for (j = 0; j < 6; j++)
			((float*)v[j]->data)[i] = particle[i].var[j];
	}
	free(particle);

	//sihuan debug
	#if 0
	for (i = 0; i < 5; i++){
		printf("%llu  ", particle[i].id);
		printf("%.5f  ", ((float*)v[0]->data)[i]);
	}
	printf("\n");
	#endif
}

size_t intersectAndsort(int64_t* preIndex, size_t preLen, SZ_VarSet* curVar, size_t dataLen, unsigned char* bitarray){
	size_t i, j, k, m, cnt;
	i = j = k = m = cnt = 0;
	SZ_Variable* v[7];
	SZ_Variable* v_tmp;
	//v[0]
	for (v_tmp = curVar->header->next, i = 0; i < 7; i++){
		v[i] = v_tmp;
		v_tmp = v_tmp->next;
	}
	for (i = 0; i < preLen; i++)
		bitarray[i] = '0';
	i = 0;
	while(i < preLen && j < dataLen){
		if (preIndex[i] == ((int64_t*)v[6]->data)[j]){
			cnt++;
			int64_t tmp;
			tmp = ((int64_t*)v[6]->data)[k];
			((int64_t*)v[6]->data)[k] = ((int64_t*)v[6]->data)[j];
			((int64_t*)v[6]->data)[j] = tmp;
			float data_tmp;
			for (m = 0; m < 6; m++){
				data_tmp = ((float*)v[m]->data)[k];
				((float*)v[m]->data)[k] = ((float*)v[m]->data)[j];
				((float*)v[m]->data)[j] = data_tmp;
			}
			k++; i++; j++;
		}
		else if (preIndex[i] < ((int64_t*)v[6]->data)[j]){
			bitarray[i] = '1';
			i++;
		}
		else j++;
	}
	printf("intersect count is: %zu, i j k pre curlen is: %zu, %zu, %zu, %zu, %zu\n\n", cnt, i, j, k, preLen, dataLen);
	return cnt;
}

void write_reordered_tofile(SZ_VarSet* curVar, size_t dataLen){
	int var_index; //0 for x, 1 for y...,3 for vx...5 for vz
	int i;
	char outputfile_name[256];
	SZ_Variable* v[7]; SZ_Variable* v_tmp;
	for (v_tmp = curVar->header->next, i = 0; i < 6; i++){
		v[i] = v_tmp;
		v_tmp = v_tmp->next;
	}
	for (var_index = 0; var_index < 6; var_index++){
		sprintf(outputfile_name, "reordered_input_%d_%d.in", sz_tsc->currentStep, var_index);
		int status_tmp;
		writeFloatData_inBytes((float*)v[var_index]->data, dataLen, outputfile_name, &status_tmp);
	}
}

float calculate_delta_t(size_t size){
	SZ_Variable* v_tmp = sz_varset->header->next;
	while(strcmp(v_tmp->varName, "x")) v_tmp = v_tmp->next;
	float* x1 = (float*) v_tmp->data;
	float* x0 = (float*) v_tmp->multisteps->hist_data;
	while(strcmp(v_tmp->varName, "vx")) v_tmp = v_tmp->next;
	float* vx0 = (float*) v_tmp->multisteps->hist_data;
	int i, j;
	double denom = 0.0;
	double div = 0.0;
	for (i = 0, j = 0; i < size; i++, j++){
		while(sz_tsc->bit_array[j] == '1') j++;
		denom += vx0[j] * (x1[i] - x0[j]);
		div   += vx0[j] * vx0[j];
	}
	printf("the calculated delta_t is: %.10f\n", denom/div);
	return denom/div;
}

int is_lossless_compressed_data(unsigned char* compressedBytes, size_t cmpSize)
{
#if ZSTD_VERSION_NUMBER >= 10300
	int frameContentSize = ZSTD_getFrameContentSize(compressedBytes, cmpSize);
	if(frameContentSize != ZSTD_CONTENTSIZE_ERROR)
		return ZSTD_COMPRESSOR;
#else
	int frameContentSize = ZSTD_getDecompressedSize(compressedBytes, cmpSize);
	if(frameContentSize != 0)
		return ZSTD_COMPRESSOR;
#endif
	
	int flag = isZlibFormat(compressedBytes[0], compressedBytes[1]);
	if(flag)
		return GZIP_COMPRESSOR;

	return -1; //fast mode (without GZIP or ZSTD)
}

unsigned long sz_lossless_compress(int losslessCompressor, int level, unsigned char* data, unsigned long dataLength, unsigned char** compressBytes)
{
	unsigned long outSize = 0; 
	size_t estimatedCompressedSize = 0;
	switch(losslessCompressor)
	{
	case GZIP_COMPRESSOR:
		outSize = zlib_compress5(data, dataLength, compressBytes, level);
		break;
	case ZSTD_COMPRESSOR:
		estimatedCompressedSize = dataLength*1.2;
		*compressBytes = (unsigned char*)malloc(estimatedCompressedSize);
		outSize = ZSTD_compress(*compressBytes, estimatedCompressedSize, data, dataLength, level); //default setting of level is 3
		break;
	default:
		printf("Error: Unrecognized lossless compressor in sz_lossless_compress()\n");
	}
	return outSize;
}

unsigned long sz_lossless_decompress(int losslessCompressor, unsigned char* compressBytes, unsigned long cmpSize, unsigned char** oriData, unsigned long targetOriSize)
{
	unsigned long outSize = 0;
	switch(losslessCompressor)
	{
	case GZIP_COMPRESSOR:
		outSize = zlib_uncompress5(compressBytes, cmpSize, oriData, targetOriSize);
		break;
	case ZSTD_COMPRESSOR:
		*oriData = (unsigned char*)malloc(targetOriSize);
		ZSTD_decompress(*oriData, targetOriSize, compressBytes, cmpSize);
		outSize = targetOriSize;
		break;
	default:
		printf("Error: Unrecognized lossless compressor in sz_lossless_decompress()\n");
	}
	return outSize;
}

unsigned long sz_lossless_decompress65536bytes(int losslessCompressor, unsigned char* compressBytes, unsigned long cmpSize, unsigned char** oriData)
{
	unsigned long outSize = 0;
	switch(losslessCompressor)
	{
	case GZIP_COMPRESSOR:
		outSize = zlib_uncompress65536bytes(compressBytes, cmpSize, oriData);
		break;
	case ZSTD_COMPRESSOR:
		*oriData = (unsigned char*)malloc(65536);
		memset(*oriData, 0, 65536);
		ZSTD_decompress(*oriData, 65536, compressBytes, cmpSize);	//the first 32768 bytes should be exact the same.
		outSize = 65536;
		break;
	default:
		printf("Error: Unrecognized lossless compressor\n");
	}
	return outSize;
}
