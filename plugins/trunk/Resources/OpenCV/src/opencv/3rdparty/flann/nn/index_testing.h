/***********************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright 2008-2009  Marius Muja (mariusm@cs.ubc.ca). All rights reserved.
 * Copyright 2008-2009  David G. Lowe (lowe@cs.ubc.ca). All rights reserved.
 *
 * THE BSD LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/

#ifndef TESTING_H
#define TESTING_H


#include "nn_index.h"
#include "matrix.h"


using namespace std;

namespace flann
{

void search_for_neighbors(NNIndex& index, const Matrix<float>& testset, Matrix<int>& result, Matrix<float>& dists, const SearchParams &searchParams, int skip = 0);

float test_index_checks(NNIndex& index, const Matrix<float>& inputData, const Matrix<float>& testData, const Matrix<int>& matches,
            int checks, float& precision, int nn = 1, int skipMatches = 0);

float test_index_precision(NNIndex& index, const Matrix<float>& inputData, const Matrix<float>& testData, const Matrix<int>& matches,
             float precision, int& checks, int nn = 1, int skipMatches = 0);

float test_index_precisions(NNIndex& index, const Matrix<float>& inputData, const Matrix<float>& testData, const Matrix<int>& matches,
                    float* precisions, int precisions_length, int nn = 1, int skipMatches = 0, float maxTime = 0);

}

#endif //TESTING_H