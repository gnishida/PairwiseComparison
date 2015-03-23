#include "PairwiseComparison.h"
#include <assert.h>

/**
 * pairwise comparisonの結果が与えられた時に、ユーザのpreferenceベクトルを計算して返却する。
 *
 * @param comparisons	[comparison ID]<選択したオプション(0/1), <オプション1のfeature, オプション2のfeature> >
 * @return				ユーザのpreferenceベクトル
 */
vector<float> PairwiseComparison::computePreferences(vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, int maxIterations, bool l1, float lambda, float eta, float threshold) {
	assert(comparisons.size() > 0);

	// 次元数
	int d = comparisons[0].second.first.size();

	// preferenceベクトルを初期化
	vector<float> preferences(d, 1.0f / (float)d);

	// Gradient descenntにより、preferenceベクトルを推定する
	gradientDescent(preferences, comparisons, maxIterations, l1, lambda, eta, threshold);
		
	return preferences;
}

/**
 * アイテムのスコアを返却する。
 *
 * @param w		ユーザpreferenceベクトル
 * @param f		アイテムのfeatureベクトル
 * @return		アイテムのスコア
 */
float PairwiseComparison::score(const vector<float>& w, const vector<float>& f) {
	return dot(w, f);
}

/**
 * Run gradient descent
 *
 * @w [OUT]			推定されたpreference vector
 * @features		学習データ　リストの各elementは、２つのfeatureベクトルのpair。
 * @choices			ラベルのリスト　これを学習する。
 * @maxIterations	最大ステップ数
 * @l1				L1 generalization termを使用するならtrue、L2ならfalse
 * @lambda			generalization termの係数
 * @eta				学習速度
 * @threshold		収束しきい値
 */
void PairwiseComparison::gradientDescent(vector<float>& w, vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, int maxIterations, bool l1, float lambda, float eta, float threshold, bool normalize) {
	//int numFeatures = features[0].first.size();
	int numComponents = comparisons[0].second.first.size();

	float curE = negativeLogLikelihood(comparisons, w, l1, lambda);
	for (int iter = 0; iter < maxIterations; ++iter) {
		std::vector<float> dw(numComponents, 0.0f);

		for (int d = 0; d < comparisons.size(); ++d) {
			float e = expf(dot(w, comparisons[d].second.second) - dot(w, comparisons[d].second.first));
			float a = comparisons[d].first - 1.0f / (1.0f + e);
			
			for (int k = 0; k < numComponents; ++k) {
				dw[k] += (comparisons[d].second.second[k] - comparisons[d].second.first[k]) * a;
			}
		}

		for (int k = 0; k < numComponents; ++k) {
			if (l1) {
				if (w[k] >= 0) {
					w[k] -= eta * (lambda + dw[k]);
				} else {
					w[k] -= eta * (-lambda + dw[k]);
				}
			} else {
				w[k] -= eta * (lambda * w[k] + dw[k]);
			}
		}

		float nextE = negativeLogLikelihood(comparisons, w, l1, lambda);
		if (curE - nextE < threshold) break;

		curE = nextE;
	}

	if (normalize) {
		float n = sqrtf(dot(w, w));
		for (int k = 0; k < numComponents; ++k) {
			w[k] /= n;
		}
	}
}

float PairwiseComparison::negativeLogLikelihood(vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, vector<float> w, bool l1, float lambda) {
	//int numFeatures = features[0].first.size();
	int numComponents = comparisons[0].second.first.size();

	float E = 0.0f;
	for (int d = 0; d < comparisons.size(); ++d) {
		float diff = dot(w, comparisons[d].second.second) - dot(w, comparisons[d].second.first);
		E += logf(1.0f + expf(diff)) + (comparisons[d].first - 1.0f) * diff;
	}

	if (l1) {
		for (int k = 0; k < numComponents; ++k) {
			E += fabs(w[k]) * lambda;
		}
	} else {
		E += dot(w, w) * lambda / 2.0f;
	}

	return E;
}

float PairwiseComparison::dot(const vector<float>& w, const vector<float>& f) {
	float ret = 0.0f;
	for (int i = 0; i < w.size(); ++i) {
		ret += w[i] * f[i];
	}

	return ret;
}