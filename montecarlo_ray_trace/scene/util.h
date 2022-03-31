#pragma once
#include<vector>
#include<algorithm>

class UnionFind {
public:
	std::vector<int> fa;
	std::vector<float> areas;
	UnionFind() {};
	UnionFind(int n) { fa.resize(n + 1, -1); areas.resize(n + 1, 0); }
	void init(int n) {
		fa.resize(n + 1, -1); areas.resize(n + 1, 0);
	}
	int find(int x) { return fa[x] < 0 ? x : fa[x] = find(fa[x]); }

	void merge(int x1, int x2) {
		int r1 = find(x1), r2 = find(x2);
		if (r1 != r2) {
			if (fa[r1] > fa[r2]) std::swap(r1, r2);
			fa[r1] += fa[r2];
			fa[r2] = r1;
			areas[r1] += areas[r2];
		}
	}
	bool ask(int a, int b) {
		return find(a) == find(b);
	}
	int size(int x) { return -fa[find(x)]; }
};
