#ifndef DIFF_H
#define DIFF_H

#include <vector>
#include <string>

namespace diff {

typedef std::vector<std::vector<int> > diff_matrix;

template<typename element>
diff_matrix lcs_length(std::vector<element> const &X,std::vector<element> const &Y)
{
	int m=X.size();
	int n=Y.size();
	diff_matrix C(m+1,std::vector<int>(n+1,0));
	int i,j;
	for(i=1;i<=m;i++) {
		for(j=1;j<=n;j++) {
			if(X[i-1]==Y[j-1]) {
				C[i][j]=C[i-1][j-1]+1;
			}
			else {
				C[i][j]=std::max(C[i][j-1],C[i-1][j]);
			}
		}
	}
	return C;
}

template<typename element,typename CNT>
void print_diff(diff_matrix const &C,std::vector<element> const &X,std::vector<element> const &Y,int i,int j,CNT &out)
{
	if(i>0 && j>0 && X[i-1]==Y[j-1]){
		print_diff(C,X,Y,i-1,j-1,out);
		out.push_back(make_pair(0,X[i-1]));
	}
	else {
		if(j>0 && (i==0 || C[i][j-1] >= C[i-1][j])){
			print_diff(C,X,Y,i,j-1,out);
			out.push_back(make_pair(1,Y[j-1]));
		}
		else if(i > 0 && (j == 0 || C[i][j-1] < C[i-1][j])) {
			print_diff(C,X,Y,i-1,j,out);
			out.push_back(make_pair(-1,X[i-1]));
		}
	}
}

template<typename element,typename CNT>
void diff(std::vector<element> const &X,std::vector<element> const &Y,CNT &out)
{
	diff_matrix C=diff::lcs_length(X,Y);
print_diff(C,X,Y,X.size(),Y.size(),out);
}

} // namespace diff

std::vector<std::string> split(std::string const &s)
{
	std::vector<std::string> res;
	size_t pos=0,pos2=0;
	while((pos2=s.find('\n',pos))!=std::string::npos) {
		res.push_back(s.substr(pos,pos2-pos));
		pos=pos2+1;
	}
	res.push_back(s.substr(pos));
	return res;
}



#endif
