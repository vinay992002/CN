#include <bits/stdc++.h>
using namespace std;
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define parity 1  //1 for odd parity and 2 for even parity
int main(){
	string s;
	cin>>s;
	int nc = s.length(); 
	long long int data = 0;
	int y,ty,tey;
	//cout<<nc<<endl;
	for(int i=0;i<nc;i++)
	{
		data<<=4;
		ty=s[i]-'0';
		if(s[i]>='A')
		{
			 ty = s[i]-'A'+10;
		}
		//cout<<ty<<"  ";
		for(int j=0;j<4;j++)
		{

			if(CHECK_BIT(ty,j))
			{
				tey =1;
				tey<<=j;
				data|=tey;
			}
		}
		
	}

	//cout<<data<<endl;
	int r=0;
	while(pow(2,r)< nc+r+1)r++;
	map<int,vector<int> > m;
	for(int i=0;i<r;i++)
	{
		for(int j=1;j<nc+r;j++)
		{
			if(CHECK_BIT(j,i))
			{
				m[i+1].push_back(j);
			}
		}
	}
	long long int mdata=0;
	int x=0;
	int temp;
	for(int i=2;i<=nc+r;i++)
	{

		if((i&(i-1)) != 0)
		{

			temp=0;
			if(CHECK_BIT(data,x))
			{
				temp=1;
				temp<<=(i-1);
			}
			mdata = mdata|temp;
			x++;
		}
	}
	//cout<<x<<"   ";
	//cout<<mdata<<endl;
	map<int ,vector<int> > ::iterator it;
	vector<int> vec;
	int count ;
	for(it = m.begin();it!=m.end();it++)
	{
		vec = it->second;
		count =0;
		//cout<<it->first<<"-->";
		for(int i=0;i<vec.size();i++)
		{
			if(CHECK_BIT(mdata,vec[i]-1))
			{
				count++;
			}
			
		}
		//cout<<count<<" ";
		//cout<<endl;
		if(parity==1 && count%2==0 )
		{
			temp=1;
			temp= temp << (int)(pow(2,it->first-1)-1);
			mdata = mdata|temp;
		}
		else if(parity==2 && count%2!=0)
		{
			temp=1;
			temp= temp << (int)(pow(2,it->first-1)-1);
			mdata = mdata|temp;
		}
		//cout<<mdata<<" ";
	}
	long long int rdata=mdata;
	vec.clear();
	//cout<<"\n\n";
	while(rdata>0)
	{
		y = rdata&1;
		vec.push_back(y);
		rdata>>=1;
	}	
	for(int i=vec.size()-1;i>=0;i--)
	{
		cout<<vec[i];
	}
	cout<<endl;
	//putting Error at Random position and detecting that position
	y =rand()%(nc+r);
	y =rand()%(nc+r);
	while(CHECK_BIT(mdata,y)!=0)y=rand()%(nc+r);
	temp=1;
	temp<<=y;
	mdata|=temp;
	y=0;
	 rdata=mdata;
	vec.clear();
	cout<<"Erronous Data : ";
	while(rdata>0)
	{
		y = rdata&1;
		vec.push_back(y);
		rdata>>=1;
	}	
	for(int i=vec.size()-1;i>=0;i--)
	{
		cout<<vec[i];
	}
	cout<<endl;
	//position is deleted. I haven't stored it any other variable
	int ep=0;
	count=0;
	for(it=m.begin();it!=m.end();it++){
		vec = it->second;
		count=0;
		for(int i=0;i<vec.size();i++){
				if(CHECK_BIT(mdata,vec[i]-1))
					count++;
			
		}
		if(parity==1&& count%2==0){
				ep+=pow(2,it->first-1);
		}
		else if(parity == 2 && count%2!=0){
			ep+=pow(2,it->first-1);
		}
	}
	cout<<"Error possition(from last) :"<<ep<<endl;
}