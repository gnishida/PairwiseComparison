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
#include "PairwiseComparison2.h"

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

void checkEstimations(vector<int>& rankings, vector<vector<float> >& features, vector<float>& preference) {
	int correct = 0;
	int incorrect = 0;

	for (int i = 0; i < rankings.size(); ++i) {
		float score1 = PairwiseComparison::dot(preference, features[i]);
		for (int j = i + 1; j < rankings.size(); ++j) {
			float score2 = PairwiseComparison::dot(preference, features[j]);

			int choice = rankings[i] < rankings[j] ? 1 : 0;
			int choice2 = score1 > score2 ? 1 : 0;
			if (choice == choice2) {
				correct++;
			} else {
				incorrect++;
			}
		}
	}

	cout << "Accuracy: " << (float)correct / (correct + incorrect) << endl;
}

double dist(vector<float>& a, vector<float>& b) {
	double l = 0.0;
	for (int i = 0; i < a.size(); ++i) {
		l += (a[i] - b[i]) * (a[i] - b[i]);
	}
	return sqrt(l);
}

/**
 * Active ranking using pairwise comparisonsの論文に基づき、
 * No.1アイテムからの距離でスコアを決定する。
 */
void testDistBasedScoring(vector<int>& rankings, vector<vector<float> >& features) {
	// No.1アイテムのfeatureベクトルを探す
	vector<float> best_feature;
	for (int i = 0; i < rankings.size(); ++i) {
		if (rankings[i] == 1) {
			best_feature = features[i];
		}
	}

	int correct = 0;
	int incorrect = 0;

	for (int i = 0; i < rankings.size(); ++i) {
		float dist1 = dist(best_feature, features[i]);

		for (int j = i + 1; j < rankings.size(); ++j) {
			float dist2 = dist(best_feature, features[j]);

			int choice = rankings[i] < rankings[j] ? 1 : 0;
			int choice2 = dist1 < dist2 ? 1 : 0;
			if (choice == choice2) {
				correct++;
			} else {
				incorrect++;
			}
		}
	}

	cout << "Accuracy: " << (float)correct / (correct + incorrect) << endl;

	// ランキングー距離の関係
	vector<float> dists(27);
	for (int i = 0; i < features.size(); ++i) {
		float d = dist(best_feature, features[i]);
		dists[rankings[i] - 1] = d;
	}

	// 結果をファイルに書き込む
	FILE* fp = fopen("dists.txt", "w");
	for (int i = 0; i < 27; ++i) {
		fprintf(fp, "%lf\n", dists[i]);
	}
	fclose(fp);
}

int main() {
	// ランキングの答え
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

	// featureベクトルを計算する
	auto features = generateFeatureVectors();
	//auto features = generateFeatureVectorsByQuadraticKernel();
	//auto features = generateFeatureVectorsByQuadraticKernel2();

	// pairwise comparisonsを作成する
	auto comparisons = generatePairwiseComparisons(rankings, features);

#if 0
	testDistBasedScoring(rankings, features);
#endif

	// preferenceベクトルを推定する
	vector<float> preference = PairwiseComparison2::computePreferences(comparisons, 10000, false, 0.0, 0.002, 0.00001);
	cout << "Preference: " << endl;
	for (int i = 0; i < preference.size(); ++i) {
		cout << preference[i] << ",";
	}
	cout << endl;

	// pairwise comparisonsの答え合わせ
	checkEstimations(rankings, features, preference);

	// ランキングースコアの関係
	vector<float> scores(27);
	for (int i = 0; i < features.size(); ++i) {
		float score = PairwiseComparison::dot(preference, features[i]);
		scores[rankings[i] - 1] = score;
	}

	// 結果をファイルに書き込む
	FILE* fp = fopen("scores.txt", "w");
	for (int i = 0; i < 27; ++i) {
		fprintf(fp, "%lf\n", scores[i]);
	}
	fclose(fp);

	return 0;
}
