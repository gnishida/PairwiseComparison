/**
 * Pairwise comparisonsから、ユーザのpreferenceベクトルを計算する。
 *
 * 本テストプログラムの目的：
 * あるアイテムが複数の要素で構成されているとした時、ユーザがどのアイテムをより好むかは、
 * ユーザのpreferenceベクトルとアイテムのfeatureベクトルの内積によって決定できるという
 * モデルが、本当に妥当なのか、確認したい。
 * 
 * 確認方法：
 * discrete（各要素について、高・中・小）で生成した選択肢について、総当りでpairwise comparisonを行い、
 * その結果から、gradient descentでユーザのpreferenceベクトルを求める。
 * 上の仮説が合っていれば、得られたpreferenceベクトルとfeatureベクトルの内積が、
 * pairwise comparisonの結果とほぼ一致するはずだ。
 *
 * 結果：
 * [0.571667, -0.808937, 0.137177]
 *
 * @authro	Gen Nishida
 * @date	3/19/2015
 * @version	1.0
 */

#include <iostream>
#include <vector>
#include <map>
#include "PairwiseComparison.h"
//#include "PairwiseComparison2.h"

using namespace std;

/**
 * featureベクトルをlinear kernelを使って生成する。
 *
 * @return	linear kernelのfeatureベクトル
 */
vector<vector<float> > generateFeatureVectors() {
	float val[3] = {1.0f, 0.5f, 0.0f};
	vector<vector<float> > features(27, vector<float>(3));
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				features[i * 9 + j * 3 + k][0] = val[i];
				features[i * 9 + j * 3 + k][1] = val[j];
				features[i * 9 + j * 3 + k][2] = val[k];
			}
		}
	}

	return features;
}

/**
 * featureベクトルをquadratic kernelを使って生成する。
 *
 * @return	quadratic kernelのfeatureベクトル
 */
vector<vector<float> > generateFeatureVectorsByQuadraticKernel() {
	float val[3] = {1.0f, 0.5f, 0.0f};
	vector<vector<float> > features(27, vector<float>(6));
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				features[i * 9 + j * 3 + k][0] = val[i] * val[i];
				features[i * 9 + j * 3 + k][1] = val[j] * val[j];
				features[i * 9 + j * 3 + k][2] = val[k] * val[k];
				features[i * 9 + j * 3 + k][3] = val[i] * val[j] * sqrt(2.0);
				features[i * 9 + j * 3 + k][4] = val[j] * val[k] * sqrt(2.0);
				features[i * 9 + j * 3 + k][5] = val[k] * val[i] * sqrt(2.0);
			}
		}
	}

	return features;
}

/**
 * featureベクトルをちょっと違うquadratic kernelを使って生成する。
 *
 * @return	quadratic kernelのfeatureベクトル
 */
vector<vector<float> > generateFeatureVectorsByQuadraticKernel2() {
	float val[3] = {1.0f, 0.5f, 0.0f};
	vector<vector<float> > features(27, vector<float>(6));
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				features[i * 9 + j * 3 + k][0] = val[i] * val[i];
				features[i * 9 + j * 3 + k][1] = val[j] * val[j];
				features[i * 9 + j * 3 + k][2] = val[k] * val[k];
				features[i * 9 + j * 3 + k][3] = val[i] * val[i];
				features[i * 9 + j * 3 + k][4] = val[j] * val[j];
				features[i * 9 + j * 3 + k][5] = val[k] * val[k];
			}
		}
	}

	return features;
}

/**
 * 指定されたfeatureベクトルについて、総当りのpairwise comparisonsを作成する
 *
 * @param rankings		各アイテムのランキング
 * @param features		各アイテムのfeatureベクトル
 * @return				pairwise comparisons
 */
vector<pair<int, pair<vector<float>, vector<float> > > > generatePairwiseComparisons(vector<int>& rankings, vector<vector<float> >& features) {
	vector<pair<int, pair<vector<float>, vector<float> > > > comparisons;
	for (int i = 0; i < rankings.size(); ++i) {
		for (int j = i + 1; j < rankings.size(); ++j) {
			int choice = rankings[i] < rankings[j] ? 1 : 0;
			comparisons.push_back(make_pair(choice, make_pair(features[i], features[j])));
		}
	}

	return comparisons;
}

/**
 * 予測結果を、Kendall tau距離を使って測定する。
 * Kendall tau距離とは、全てのペアについて、ランキング上の上下をチェックし、#間違いペア / ペアの総数で求まる。
 *
 * @param rankings		ランキング
 * @param features		featureベクトルのリスト
 * @param preference	ユーザのpreferenceベクトル
 */
void checkEstimations(vector<int>& rankings, vector<vector<float> >& features, vector<float>& preference) {
	int correct = 0;
	int incorrect = 0;

	for (int i = 0; i < rankings.size(); ++i) {
		float score1 = PairwiseComparison::score(preference, features[i]);
		for (int j = i + 1; j < rankings.size(); ++j) {
			float score2 = PairwiseComparison::score(preference, features[j]);

			int choice = rankings[i] < rankings[j] ? 1 : 0;
			int choice2 = score1 > score2 ? 1 : 0;
			if (choice == choice2) {
				correct++;
			} else {
				incorrect++;
			}
		}
	}

	cout << "Kendall tau: " << (float)incorrect / (correct + incorrect) << endl;
}

double dist(vector<float>& a, vector<float>& b) {
	double l = 0.0;
	for (int i = 0; i < a.size(); ++i) {
		l += (a[i] - b[i]) * (a[i] - b[i]);
	}
	return sqrt(l);
}

int main() {
	// ランキングの答え
	vector<vector<int> > rankings;
	FILE* fp = fopen("comparisons.txt", "r");
	while (true) {
		vector<int> ranking(27);
		char buff[256];
		if (!fgets(buff, 256, fp)) break;
		sscanf(buff, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", 
			&ranking[0], &ranking[1], &ranking[2], &ranking[3], &ranking[4], &ranking[5], &ranking[6], &ranking[7], &ranking[8], 
			&ranking[9], &ranking[10], &ranking[11], &ranking[12], &ranking[13], &ranking[14], &ranking[15], &ranking[16], &ranking[17],
			&ranking[18], &ranking[19], &ranking[20], &ranking[21], &ranking[22], &ranking[23], &ranking[24], &ranking[25], &ranking[26]);
		rankings.push_back(ranking);
	}
	/*
	vector<int> rankings(27);
	rankings[0] = 19;
	rankings[1] = 20;
	rankings[2] = 21;
	rankings[3] = 3;
	rankings[4] = 4;
	rankings[5] = 6;
	rankings[6] = 1;
	rankings[7] = 2;
	rankings[8] = 5;
	rankings[9] = 22;
	rankings[10] = 23;
	rankings[11] = 24;
	rankings[12] = 10;
	rankings[13] = 11;
	rankings[14] = 12;
	rankings[15] = 7;
	rankings[16] = 8;
	rankings[17] = 9;
	rankings[18] = 25;
	rankings[19] = 26;
	rankings[20] = 27;
	rankings[21] = 16;
	rankings[22] = 17;
	rankings[23] = 18;
	rankings[24] = 13;
	rankings[25] = 14;
	rankings[26] = 15;
	*/

	// featureベクトルを計算する
	auto features = generateFeatureVectors();
	//auto features = generateFeatureVectorsByQuadraticKernel();
	//auto features = generateFeatureVectorsByQuadraticKernel2();

	// pairwise comparisonsを作成する
	for (int u = 0; u < rankings.size(); ++u) {
		auto comparisons = generatePairwiseComparisons(rankings[u], features);

		// preferenceベクトルを推定する
		vector<float> preference = PairwiseComparison::computePreferences(comparisons, 10000, false, 0.1, 0.002, 0.00001);
		cout << "Preference: ";
		for (int i = 0; i < preference.size(); ++i) {
			cout << preference[i] << ",";
		}
		cout << endl;

		/*
		// simple modelの場合、preferenceベクトルはとりあえずハードコーディング
		vector<float> preference(3);
		preference[0] = 0.2;
		preference[1] = -0.4;
		preference[2] = 0.1;
		*/

		// pairwise comparisonsの答え合わせ
		checkEstimations(rankings[u], features, preference);

		// ランキングースコアの関係
		vector<float> scores(27);
		for (int i = 0; i < features.size(); ++i) {
			float s = PairwiseComparison::score(preference, features[i]);
			scores[rankings[u][i] - 1] = s;
		}

		// 結果をファイルに書き込む
		char filename[256];
		sprintf(filename, "scores_%d.txt", u);
		FILE* fp = fopen(filename, "w");
		for (int i = 0; i < 27; ++i) {
			fprintf(fp, "%lf\n", scores[i]);
		}
		fclose(fp);
	}

	return 0;
}
