#include <bits/stdc++.h>
using namespace std;
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
int main(){
	long long int data,cdata;
	int polkey;
	string s;

	cin>>s;
	polkey=0;
	int y=0;
	for(int i=0;i<s.length();i++)
	{
		y=0;
		y = s[i]-'0';
		//cout<<y<<"  ";
		polkey<<=1;
		polkey = polkey|y;
	}
	
	//cout<<polkey<<endl;
	cin>>s;
	  y=0;
	data =0;
	for(int i=0;i<s.length();i++)
	{
		y=0;
		y = s[i]-'0';
		data<<=1;
		data = data|y;
	}
	//cout<<data<<endl;
	cdata = data;
	int temp;
	temp= polkey;
	int c=0;
	int nc=0;
	while(cdata>0)
	{
		cdata>>=1;
		//cout<<cdata<<" ";
		nc++;
	}
	//cout<<endl;
	//cout<<nc<<endl;
	cdata = data;
	while(temp>0){
		c++;
		temp = temp>>1;
	}
	//cout<<c<<endl;
		c--;
		nc+=c;
		cdata= cdata<<c;
		//cout<<cdata;
		c++;
	int cc=0;
	temp=0;
	cc=nc-1;
	int xc=c-1;
	while(c>0)
	{
		temp<<=1;
		if(CHECK_BIT(cdata,cc))
		{
			temp=temp|1;
		}
		cc--;
		c--;
	}
	int carry=0;
	cout<<temp;
    temp = temp^polkey;
    cout<<"\n"<<temp<<endl;
	while(cc>=0)
	{
		temp<<=1;
		if(CHECK_BIT(cdata,cc))
		{
			temp = temp|1;
		}
		if(CHECK_BIT(temp,xc))
		{
			temp=temp^polkey;
		}
		else
		{
			temp=temp^0;
		}
		//cout<<temp<<" ";
		cc--;
	}
	cout<<"CRC is :";
	cout<<temp<<endl;
	cout<<"thus data is :";
	cdata = cdata|temp;
	stack<int> st;
	while(cdata>0){
		st.push(cdata&1);
		cdata>>=1;
	}
	while(!st.empty()){
		cout<<st.top();
		st.pop();
	}
	cout<<endl;

	
}