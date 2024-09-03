#include <bits/stdc++.h>
#include <thread>
#include <future>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <mutex>
#define ll long long
#define pb push_back
#define vi vector<int>
#define vvi vector<vector<int>>
#define fr(a,b) for(int i = a; i < b; i++)
#define frn(a,b) for(int i = a; i >= b; i--)
#define frr(a,b,c) for(int i = a; i <= b &&c; i++)
#define frrn(a,b,c) for(int i = a; i >=b &&c; i--)
#define rep(i,a,b) for(int i = a; i < b; i++)
#define mod 1000000007
#define inf (1LL<<60)
#define all(x) (x).begin(), (x).end()
#define prDouble(x) cout << fixed << setprecision(10) << x
#define triplet pair<ll,pair<ll,ll>>
#define fast_io ios_base::sync_with_stdio(false);cin.tie(NULL)
using namespace std;


class Stock {
private:
    vector<double> calculateEMA(const vector<double>& closePrices, int period) {
    vector<double> ema;
    if (closePrices.size() < period) {
        cerr << "Error: Not enough data to calculate EMA" << endl;
        return ema;
    }
    double multiplier = 2.0 / (period + 1);
    vector<double> emaValues;
    double emaPrev = 0.0;
    double sum = 0.0;
    for (int i = 0; i < period; ++i) {
        sum += closePrices[i];
    }
    emaPrev = sum / period;
    emaValues.push_back(emaPrev);
    for (size_t i = period; i < closePrices.size(); ++i) {
        emaPrev = (closePrices[i] - emaPrev) * multiplier + emaPrev;
        emaValues.push_back(emaPrev);
    }
    ema.insert(ema.end(), period - 1, 0);
    ema.insert(ema.end(), emaValues.begin(), emaValues.end());
    return ema;
}
    string convertToDDMMYYYY(const string& date) {
    stringstream ss(date);
    string year, month, day;
    getline(ss, year, '-');
    getline(ss, month, '-');
    getline(ss, day, '-');
    return day + "-" + month + "-" + year;
 }

     vector<double> calculateATH(const vector<double>& closePrices, int period) {
        vector<double> ATH;
        if (closePrices.size() < period) {
            cerr << "Error: Not enough data to calculate ATH" << endl;
            return ATH;
        }
        deque<int> dq; 
        for (int i = 0; i < closePrices.size(); ++i) {
            while (!dq.empty() && closePrices[dq.back()] <= closePrices[i]) {
                dq.pop_back();
            }
            dq.push_back(i);
            if (!dq.empty() && dq.front() <= i - period) {
                dq.pop_front();
            }
            if (i >= period - 1) {
                ATH.push_back(closePrices[dq.front()]);
            } else {
                ATH.push_back(0);  
            }
        }
        return ATH;
    }


    string extractYYYYMMDD(const string& dateStr) {
    int day, month, year;
    char dash;
    stringstream ss(dateStr);
    ss >> day >> dash >> month >> dash >> year;
    stringstream formattedDate;
    formattedDate << setw(4) << setfill('0') << year
                  << setw(2) << setfill('0') << month
                  << setw(2) << setfill('0') << day;

    return formattedDate.str();
}



vector<pair<string, double>> calculate12MReturns(const vector<double>& closePrices, const vector<string>& dates) {
    vector<pair<string, double>> returns;
    vector<string> formattedDates;
    map<pair<int, int>, vector<pair<int, double>>> dataMap;
    for (const auto& date : dates) {
        formattedDates.push_back(extractYYYYMMDD(date));
    }
    int n = dates.size();
    for (int i = 0; i < n; ++i) {
        string dateStr = formattedDates[i];
        int year = stoi(dateStr.substr(0, 4));
        int month = stoi(dateStr.substr(4, 2));
        int day = stoi(dateStr.substr(6, 2));
        dataMap[{year, month}].emplace_back(day, closePrices[i]);
    }
    for (int year = 2016; year <= 2024; ++year) {
        for (int month = 1; month <= 12; ++month) {
            auto upcomingYearData = dataMap.find(month == 1 ? make_pair(year , 12) : make_pair(year+1, month - 1));
            if (upcomingYearData != dataMap.end()) {
                auto lastTradingDayUpcoming = upcomingYearData->second.back(); 
                double upcomingYearClosePrice = lastTradingDayUpcoming.second;
                auto currYearData = dataMap.find({year, month});
                if (currYearData != dataMap.end()) {
                    auto firstTradingDayCurrent = currYearData->second.front(); 
                    double currYearClosePrice = firstTradingDayCurrent.second;
                    double returnRate = (upcomingYearClosePrice - currYearClosePrice) / currYearClosePrice;
                    int day = firstTradingDayCurrent.first;
                    string currDate = to_string(year) + "-" + (month < 10 ? "0" : "") + to_string(month) + "-" + (day < 10 ? "0" : "") + to_string(day);
                    returns.push_back(make_pair(currDate, returnRate));
                }
            }
        }
    }
    return returns;
}

vector<pair<string, pair<double,double>>> calculate1MReturns(const vector<double>& closePrices, const vector<string>& dates, const string& name) {
    vector<pair<string, pair<double,double>>> returns; 
    vector<string> formattedDates;
    map<pair<int, int>, vector<pair<int, double>>> dataMap;
    for (const auto& date : dates) {
        formattedDates.push_back(extractYYYYMMDD(date));
    }
    int n = dates.size();
    for (int i = 0; i < n; ++i) {
        string dateStr = formattedDates[i];
        int year = stoi(dateStr.substr(0, 4));
        int month = stoi(dateStr.substr(4, 2));
        int day = stoi(dateStr.substr(6, 2));
        dataMap[{year, month}].emplace_back(day, closePrices[i]);
    }
    for (int year = 2016; year <= 2024; ++year) {
        for (int month = 1; month <= 12; ++month) {
             int upcomingmonth=(month==12?1:month+1);
            int upcomingyear=(month==12?year+1:year);
            auto currMonthData = dataMap.find({year, month});
              auto nextMonthData = dataMap.find({upcomingyear, upcomingmonth});
            if (currMonthData != dataMap.end() && nextMonthData!=dataMap.end()) {
                auto firstTradingDayCurrent = currMonthData->second.front(); 
                auto lastTradingDayCurrent = currMonthData->second.back();
               

                auto lasttradingdayofnextmonth=nextMonthData->second.back();
                
                double currPrice = firstTradingDayCurrent.second;
                double endofMonthPrice = lastTradingDayCurrent.second;

                double endofnextmonthprice=lasttradingdayofnextmonth.second;

                double returnRate = (endofMonthPrice - currPrice) / currPrice;
                double returnrateofcontinuedstock=(endofnextmonthprice-endofMonthPrice)/endofMonthPrice;
                int day = firstTradingDayCurrent.first;
                string currDate = to_string(year) + "-" + (month < 10 ? "0" : "") + to_string(month) + "-" + (day < 10 ? "0" : "") + to_string(day);
                if (!isnan(returnRate) && !isinf(returnRate) ) {
                    returns.push_back(make_pair(currDate,make_pair( returnrateofcontinuedstock,returnRate)));  //first 31-31 //second 1-31
                }
                else 
                {
                    returns.push_back(make_pair(currDate, make_pair(-20,-20)));
                }
            }
        }
    }
    return returns;
}


string convertDateFormat(const string& date) {
    stringstream dateStream(date);
    string month, day, year;
    
    if (getline(dateStream, month, '-') &&
        getline(dateStream, day, '-') &&
        getline(dateStream, year)) {
        return day + '-' + month + '-' + year;
    } else {
        cerr << "Error parsing date: " << date << endl;
        return ""; 
    }
}


  vector<pair<string,long long int>> ptimesvolume( vector<pair<string, double>> prices,    vector<pair<string,double>> volumes)
  {
      vector<pair<string,long long int>> answer;
     int j=0;
      for(auto it: volumes)
      {
          if(convertDateFormat(it.first)==prices[0].first)
          break;
          else
          j++;
      }
int i=0;
    while(j!=-3 && i<prices.size() && j<volumes.size())
    {
        auto it=volumes[j];
        auto itp=prices[i];
        if(it.first.length()==0)
        {
            j=-3;
        }
        else if(convertDateFormat(it.first)!=itp.first)
        {
            cout<<" error in date match at idx"<< i <<endl;
            i++;
            j++;
        }
        else
        {
            answer.pb({itp.first,itp.second*it.second});
            i++;
            j++;
        }
    }
    cout<<j<<endl;
      return answer;
  }



public:
    string name;
    vector<pair<string, double>> prices; 
    vector<pair<string,double>> volumes;
    vector<pair<string, double>> ema;
    vector<pair<string, double>> ath;
    vector<pair<string, bool>> isgood;
    vector<pair<string, double>> returns12M;    
    vector<pair<string,double>>monthwisereturnsforstock;
     vector<pair<string,pair<double, double>>> returns1M;
     vector<pair<string,pair<double,double>>>monthwisereturns1Mforstock;
     vector<pair<string,long long int>> priceintovolume;

     void testfun()
     {
        priceintovolume=ptimesvolume(prices,volumes);
     }


     void displayVolumes() const {
    cout << "Stock: " << name << endl;
    for (const auto& entry : volumes) {
        cout << "Date: " << entry.first << ", Volume: " << entry.second << endl;
    }
}

void displayPrices() const {
    cout << "Stock: " << name << endl;
    for (const auto& entry : prices) {
        cout << "Date: " << entry.first << ", Price: " << entry.second << endl;
    }
}

int merge(const Stock& other) {
    if(name!=other.name)
    {
        cout<<-1<<endl;
        return -1;
    }
    for (auto it = other.prices.begin(); it != other.prices.end(); ++it) {
        const string& date = it->first;
        double price = it->second;
        addPrice(date, price);
    }
    return 1;
}

    Stock(const string& stockName) : name(stockName) {}
    void addPrice(const string& date, double price) {
        prices.pb(make_pair(date, price));
    }
       void addVolume(const string& date, long long volume) {
        volumes.pb(make_pair(date, volume));
    }

    void calculateEMAForStock(int period) {
        vector<double> closePrices;
        vector<string> dates;
        for (const auto& entry : prices) {
            dates.pb(entry.first);
            closePrices.pb(entry.second);
        }

        vector<double> emaValues = calculateEMA(closePrices, period);
        for (size_t i = 0; i < emaValues.size(); ++i) {
            ema.pb(make_pair(dates[i], emaValues[i]));
        }
    }

    void calculateATHForStock(int period) {
        vector<double> closePrices;
        vector<string> dates;
        for (const auto& entry : prices) {
            dates.pb(entry.first);
            closePrices.pb(entry.second);
        }
        vector<double> athValues = calculateATH(closePrices,period);
        for (size_t i = 0; i < athValues.size(); ++i) {
            ath.pb(make_pair(dates[i], athValues[i]));
        }
    }


void return12months() {
    vector<double> closePrices;
    vector<string> dates;
    
    for (const auto& entry : prices) {
        dates.pb(entry.first);
        closePrices.pb(entry.second);
    }
    vector<pair<string, double>> result = calculate12MReturns(closePrices, dates);
    monthwisereturnsforstock=result;
    unordered_map<string, double> resultMap;
    for (const auto& entry : result) {
        string convertedDate = convertToDDMMYYYY(entry.first);  
        resultMap[convertedDate] = entry.second;
    }

    for (const auto& date : dates) {
        if (resultMap.find(date) != resultMap.end() && resultMap[date] != 0) {
            returns12M.pb(make_pair(date, resultMap[date]));
        } else {
            returns12M.pb(make_pair(date, -1)); 
        }
    }
   }  
    double getPriceByDate(const string& queryDate) const {
        for (const auto& entry : prices) {
            if (entry.first == queryDate) {
                return entry.second;
            }
        }
        throw runtime_error("Date not found for this stock.");
    }


    void return1Month() {
    vector<double> closePrices;
    vector<string> dates;
    string nme=name;
    for (const auto& entry : prices) {
        dates.pb(entry.first);
        closePrices.pb(entry.second);
    }
    vector<pair<string, pair<double,double>>> result =calculate1MReturns(closePrices, dates,nme);
      monthwisereturns1Mforstock=result;
    unordered_map<string, pair<double,double>> resultMap;
    for (const auto& entry : result) {
        string convertedDate = convertToDDMMYYYY(entry.first);
        resultMap[convertedDate].first = entry.second.second;  // 1monthreturn 1-31
        resultMap[convertedDate].second=entry.second.first;    //1month returns 31-31
    }

    for (const auto& date : dates) {
        if (resultMap.find(date) != resultMap.end()) {
            returns1M.pb(make_pair(date, resultMap[date]));
        } else {
            returns1M.pb(make_pair(date, make_pair(-1,-1))); 
        }
    }
}
};

//class ends


//helperfunctions used in int main
string toLower(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c) { return tolower(c); });
    return lowerStr;
}



vector<pair<double,pair<string,string>>> monthreturns12forastock(Stock stocks){
vector<pair<double,pair<string,string>>>ans ;
    string name=stocks.name;
    auto returns12m=stocks.monthwisereturnsforstock;
    for(auto it:returns12m)
    {
        string date=it.first;
        double returns=it.second;
        ans.pb({returns,{date,name}});
    }
return ans;
}



vector<pair<pair<double,double>,pair<string,string>>> monthreturns1forastock(Stock stocks){
vector<pair<pair<double,double>,pair<string,string>>>ans ;
    string name=stocks.name;
    auto returns1m=stocks.monthwisereturns1Mforstock;
    for(auto it:returns1m)
    {
        string date=it.first;
        double returns=it.second.second;
        double returnsoncontibuedstock=it.second.first;

        ans.pb({{returns,returnsoncontibuedstock},{date,name}});
    }
return ans;
}


string convertToDDMMYYYY(const string& date) {
    stringstream ss(date);
    string year, month, day;
    getline(ss, year, '-');
    getline(ss, month, '-');
    getline(ss, day, '-');
    return day + "-" + month + "-" + year;
 }


 
string extractYYYYMMDD(const string& dateStr) {
    int day, month, year;
    char dash;
    stringstream ss(dateStr);
    ss >> day >> dash >> month >> dash >> year;
    stringstream formattedDate;
    formattedDate << setw(4) << setfill('0') << year
                  << setw(2) << setfill('0') << month
                  << setw(2) << setfill('0') << day;

    return formattedDate.str();
}

map<string,vector<pair<double,string>>> fun1(vector<vector<pair<double,pair<string,string>>>> data){

    map<string,vector<pair<double,string>>> mpp;
       for(int i=0;i<data.size();++i)
       {
           for(int j=0;j<data[i].size();++j)
           {
              auto it=data[i][j];
              double returns=it.first*100;
              string date=it.second.first;


              string name=it.second.second;
              mpp[date].pb({returns,name});
           }
       }
       return mpp;
}




map<string, vector<pair<double, string>>> fun2(map<string, vector<pair<double, string>>> datewisedatafor12monthsreturns) {
    map<string, vector<pair<double, string>>> mpp;

    for (const auto& entry : datewisedatafor12monthsreturns) {
        vector<pair<double, string>> sorted_pairs = entry.second;
        sort(sorted_pairs.begin(), sorted_pairs.end(), [](const pair<double, string>& a, const pair<double, string>& b) {
            if (isnan(a.first)) return false; 
            if (isnan(b.first)) return true;
            if (a.first == numeric_limits<double>::infinity()) return false;  
            if (b.first == numeric_limits<double>::infinity()) return true;
            return a.first > b.first;  
        });

        vector<pair<double, string>> valid_pairs;
        for (const auto& pair : sorted_pairs) {
            if (pair.first != numeric_limits<double>::infinity() && !isnan(pair.first)) {
                valid_pairs.push_back(pair);  
            }
        }
        if (!valid_pairs.empty()) {
            mpp[entry.first] = valid_pairs;
        }
    }

    return mpp;
}




map<string, vector<pair<double, string>>> fun3(int percent, map<string, vector<pair<double, string>>> sorteddata) {
    map<string, vector<pair<double, string>>> ans;
     for(auto it: sorteddata)
     {
        string date=it.first;
        auto data=it.second;
        int sz=data.size();

        int noofentries=sz*percent/100;

        vector<pair<double, string>> temp;

        for(int i=0;i<noofentries;++i)
        {
            auto itr=data[i];
            temp.pb({itr.first,itr.second});
        }
        ans[date]=temp;
     }
    return ans;
}



map<pair<int, int>, vector<int>> createDateMap(const vector<string>& dates) {
    map<pair<int, int>, vector<int>> dateMap;
    for (const auto& date : dates) {
        string formattedDate = extractYYYYMMDD(date);
        int year = stoi(formattedDate.substr(0, 4));
        int month = stoi(formattedDate.substr(4, 2));
        int day = stoi(formattedDate.substr(6, 2));
        dateMap[{year, month}].push_back(day);
    }
    return dateMap;
}


void extractDateComponents(const string& dateStr, int& day, int& month, int& year) {
    stringstream ss(dateStr);
    char dash;
    ss >> year >> dash >> month >> dash >> day;
}

string formatDate(int day, int month, int year) {
    stringstream ss;
    ss << setw(2) << setfill('0') << day << '-'
       << setw(2) << setfill('0') << month << '-'
       << setw(4) << setfill('0') << year;
    return ss.str();
}
int findLastTradingDay(const vector<int>& days) {
    if (days.empty()) return -1;
    return days.back(); 
}

int findFirstTradingDay(const vector<int>& days) {
    if (days.empty()) return -1;
    return days.front(); 
}

string find_futuredate( map<pair<int,int>,vector<int>> mappp,string date)
{
    int day, month, year;
    extractDateComponents(date, day, month, year);
    int upcomingmonth,upcomingyear;
    if(month==1)
    {
        upcomingmonth=12;
         upcomingyear=year;

    }
    else
    {
     upcomingmonth=month-1;
     upcomingyear=year+1;
    }
      auto it = mappp.find({upcomingyear, upcomingmonth});
    if (it != mappp.end()) {
        int lastTradingDay = findLastTradingDay(it->second);
        if (lastTradingDay != -1) {
            return formatDate(lastTradingDay, upcomingmonth, upcomingyear);
        }
    }
     return "";
};

string find_futuredate2( map<pair<int,int>,vector<int>> mappp,string date)
{
    int day, month, year;
    extractDateComponents(date, day, month, year);
    int upcomingmonth,upcomingyear;
 upcomingmonth=month;
 upcomingyear=year+1;
      auto it = mappp.find({upcomingyear, upcomingmonth});
    if (it != mappp.end()) {
        int firstTradingDay = findFirstTradingDay(it->second);
        if (firstTradingDay != -1) {
            return formatDate(firstTradingDay, upcomingmonth, upcomingyear);
        }
    }
     return "";
};

string find_futuredate3( map<pair<int,int>,vector<int>> mappp,string date)
{
    int day, month, year;
    extractDateComponents(date, day, month, year);
    int upcomingmonth,upcomingyear;
 upcomingmonth=month;
 upcomingyear=year+1;
      auto it = mappp.find({upcomingyear, upcomingmonth});
    if (it != mappp.end()) {
        int lastTradingDay = findLastTradingDay(it->second);
        if (lastTradingDay != -1) {
            return formatDate(lastTradingDay, upcomingmonth, upcomingyear);
        }
    }
     return "";
};


string find_lasttradingday( map<pair<int,int>,vector<int>> mappp,string date)
{
    int day, month, year;
    extractDateComponents(date, day, month, year);
      auto it = mappp.find({year, month});
    if (it != mappp.end()) {
        int lastTradingDay = findLastTradingDay(it->second);
        if (lastTradingDay != -1) {
            return formatDate(lastTradingDay, month, year);
        }
    }
     return "";
};



string convertToYYYYMMDD(const string& date) {
    stringstream ss(date);
    string day, month, year;
    getline(ss, day, '-');
    getline(ss, month, '-');
    getline(ss, year, '-');
    return year + "-" + month + "-" + day;
}

long long findavgpriceintovol(vector<pair<string, long long>> priceintovol,int start,int end)
{
    // cout<<start <<" "<<end<<endl;
   unsigned long long ans=0;
   int nooftradingdays=end-start;
//    cout<<nooftradingdays<<endl;
    for(int i=start;i<=end;++i)
    {
        ans+=priceintovol[i].second/nooftradingdays;
    }
   if(nooftradingdays>=14 && nooftradingdays<=25)
    return ans;

    else return 100000001;
}

  
map<string, vector<pair<bool, string>>> fun4 (
    map<string, vector<pair<double, string>>> &topperccentstocksdatewise, 
    vector<Stock> stocks, 
    int criteriaforath
) {
    map<string, vector<pair<bool, string>>> answer;
    map<string, int> dateToIndexMap;
    vector<pair<string, double>> p = stocks[0].prices; 
    vector<string> dates;  
    
    for (int i = 0; i < p.size(); ++i) {
        string date = convertToDDMMYYYY(p[i].first);
        dateToIndexMap[date] = i; 
        dates.push_back(p[i].first); 
    }
    
    map<pair<int,int>,vector<int>> mappp = createDateMap(dates);
    
    map<string, string> futuredates;
    map<string, pair<string, string>> futuredates2;

    for (auto& itr : topperccentstocksdatewise) {
        string currdate = itr.first;
        string newdate = find_futuredate(mappp, currdate); 
        string newdate1 = find_futuredate2(mappp, currdate); 
        string newdate2 = find_futuredate3(mappp, currdate); 
        
        futuredates[currdate] = newdate; 
        futuredates2[currdate] = make_pair(newdate1, newdate2); 
    }

    // Process each date sequentially
    for (auto it : topperccentstocksdatewise) {
        string date = it.first; 
        int i1 = -1, j1 = -1;
        auto datess = futuredates2.at(date);
        if (dateToIndexMap.find(convertToYYYYMMDD(datess.first)) != dateToIndexMap.end() && 
            dateToIndexMap.find(convertToYYYYMMDD(datess.second)) != dateToIndexMap.end()) {
            i1 = dateToIndexMap[convertToYYYYMMDD(datess.first)]; 
            j1 = dateToIndexMap[convertToYYYYMMDD(datess.second)]; 
        }

        string futureDate = futuredates.at(date); 
        vector<pair<bool, string>> tmp;

        int j = -1;
        if (dateToIndexMap.find(convertToYYYYMMDD(futureDate)) != dateToIndexMap.end()) {
            j = dateToIndexMap[convertToYYYYMMDD(futureDate)]; 
        }

        if (j == -1 || j1 == -1 || i1 == -1) continue;
         
        for (const auto& stockPair : it.second) {
            string stockName = stockPair.second;

            auto itr = find_if(stocks.begin(), stocks.end(), [&stockName](const Stock& stock) {
                return stock.name == stockName;
            });

            if (itr != stocks.end()) {
                auto prices = itr->prices; 
                auto ema = itr->ema; 
                auto ath = itr->ath; 
                auto priceintovol = itr->priceintovolume; 
                
                long long avg = findavgpriceintovol(priceintovol, i1, j1);
                
                bool cond = (prices[j].second > ema[j].second && prices[j].second >= ath[j].second * criteriaforath / 100 && avg >= 10000001);
                tmp.push_back({cond, stockName});
            }
        }
        answer[date] = tmp; 
    }

    return answer; 
}


map<string,vector<pair<pair<double,double>,string>>> fun5(vector<vector<pair<pair<double,double>,pair<string,string>>>> data){
    map<string,vector<pair<pair<double,double>,string>>> mpp;
       for(int i=0;i<data.size();++i)
       {
           for(int j=0;j<data[i].size();++j)
           {
              auto it=data[i][j];
              double returns=it.first.first*100;
              double returnsoncontinuedstock=it.first.second*100;
              string date=it.second.first;
              string name=it.second.second;
              mpp[date].pb({{returns,returnsoncontinuedstock},name});
           }
       }
       return mpp;
}


void parseDate(const string& date, int& year, int& month, int& day) {
    sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);
}

string finddate(string currdate, map<pair<int, int>, vector<int>> mappp) {
    int currYear, currMonth, currDay;
    parseDate(currdate, currYear, currMonth, currDay);
    int targetYear = currYear;
    int targetMonth = currMonth + 12;

    if (targetMonth > 12) {
        targetYear += targetMonth / 12;
        targetMonth = targetMonth % 12;
        if (targetMonth == 0) {
            targetMonth = 12;
            --targetYear;
        }
    }
    auto key = make_pair(targetYear, targetMonth);
    if (mappp.find(key) != mappp.end()) {
        int firstDay = mappp[key].front();
        return formatDate(firstDay, targetMonth, targetYear);
    } else {
        return "Date not found";
    }
}


vector<pair<string, double>> fun6(map<string, vector<pair<bool, string>>> emacheck,map<string, vector<pair<pair<double,double>, string>>> datewisedatafor1monthsreturns,int noofstocks,map<pair<int, int>, vector<int>> mappp) 
   {
   vector<pair<string, double>> answer;
    set<string> portfoliostocks;
    string bufferdate = "";  
    for (auto it : emacheck) {
        string backtestdate = it.first;
        vector<pair<bool, string>>& backtestdata = it.second;
        vector<pair<pair<double, double>, string>> futureData2;
        string future_date = finddate(backtestdate, mappp);
        map<string, double> returnsmap;
        map<string, double> continuedreturnsmap;
        double sum=0;
        double avaragereturns=0;
        if (datewisedatafor1monthsreturns.find(convertToYYYYMMDD(future_date)) != datewisedatafor1monthsreturns.end()) {
            vector<pair<pair<double, double>, string>>& futureData = datewisedatafor1monthsreturns[convertToYYYYMMDD(future_date)];
            if (bufferdate.size())
                futureData2 = datewisedatafor1monthsreturns[convertToYYYYMMDD(bufferdate)];
            for (const auto& data : futureData) {
                returnsmap[data.second] = data.first.first;
            }
            for (const auto& data : futureData2) {
                continuedreturnsmap[data.second] = data.first.second;
            }
        }
  
      int a=noofstocks;
        if (portfoliostocks.empty()) {
            auto itr = it.second;
            for (auto i : itr) {
                if(a>0)
                {
                string stock = i.second;
                bool c = i.first;
                double returnss = returnsmap[stock];
                if (c) {
                    portfoliostocks.insert(stock);
                    a--;
                    sum+=returnss;
                }
                }
                else 
                {
                    break;
                }
            }
            if(noofstocks>0)
            avaragereturns=sum/noofstocks;
            } 
        else {
            auto itr = it.second;
         
            set<string> temp;
            for (auto i : itr) {
                if(a>0)
                {
                string stock = i.second;
                bool c = i.first;

                if (portfoliostocks.find(stock) != portfoliostocks.end()) {
                    double returnss = continuedreturnsmap[stock];
                    if (c) {
                       
                        temp.insert(stock);
                        a--;
                         sum+=returnss;
                    }
                } else {
                    double returnss = returnsmap[stock];
                    if (c) {
                   
                        temp.insert(stock);
                        a--;
                         sum+=returnss;
                    }
                }
            }
            else
            {
                break;
            }
           }
            if(noofstocks>0)
            avaragereturns=sum/noofstocks;
            portfoliostocks = temp;
        }
        if(avaragereturns!=0)
        {
           string lasttradingday=find_lasttradingday(mappp,convertToYYYYMMDD(future_date));
       
        answer.pb({lasttradingday,avaragereturns});
        bufferdate = future_date;
        }
    }
    return answer;
  }

int findyear(string &date)
{
    return stoi(date.substr(6,4));
}

map<int,vector<double>> printPortfolioReturns(const vector<pair<string, double>>& portfolioreturns,double initialAmount,int month,int year,int endmonth,int endyear,map<pair<int,int>,vector<int>> mappp) {
    int tradingday=mappp[{year,month}].back();
    int tradingday2=mappp[{endyear,endmonth}].back();
    int firstday=mappp[{year,month}].front();
    map<int,vector<double>> answer;
    
    double amount = initialAmount;
     string date = (tradingday < 10 ? "0" : "") + to_string(tradingday) + "-" +
                       (month < 10 ? "0" : "") + to_string(month) + "-" +
                       to_string(year);

    string firstdate = (firstday < 10 ? "0" : "") + to_string(firstday) + "-" +
                       (month < 10 ? "0" : "") + to_string(month) + "-" +
                       to_string(year);

                       
    string date2 = to_string(tradingday2) + "-" +
                       (endmonth < 10 ? "0" : "") + to_string(endmonth) + "-" +
                       to_string(endyear);

                       cout<<date2<<endl;
                       int start=0;
                       for(int k=0;start<portfolioreturns.size();++k)
                       {
                        auto entry=portfolioreturns[k];
                        if(entry.first==date)
                        break;
                        else 
                        start++;
                       }

                       int end=0;
                        for(int k=0;end<portfolioreturns.size();++k)
                       {
                        auto entry=portfolioreturns[k];
                        if(entry.first==date2)
                        break;
                        else 
                        end++;
                       }
                       cout<<start<<" "<<end<<endl;
                       if(start>end)
                       {
                        cout<<"Please enter the start date lesser than end..."<<endl;
                        return answer;
                       }                     

    cout<<"you have started investing on : "<<firstdate<<endl;         

    cout << setw(15) << "Date" << setw(15) << "Return (%)" << setw(20) << "Portfolio Value" << endl;
    cout << "-----------------------------------------------------------" << endl;

    for (int j=start;j<end;++j) {
        auto entry=portfolioreturns[j];
        double returnPercentage = entry.second;
        amount += (amount * returnPercentage / 100.0);
        int date=findyear(entry.first);
        answer[date].pb(amount);
        std::cout << std::setw(15) << entry.first 
                  << std::setw(15) << returnPercentage 
                  << std::setw(20) << amount 
                  << std::endl;
    }
    return answer;
}


void writePortfolioReturnsToCSV(const std::vector<std::pair<std::string, double>>& portfolioreturns, double initialAmount, int month, int year, int endmonth, int endyear, map<pair<int, int>, vector<int>> mappp, int numOfStocks) {
    int tradingday = mappp[{year, month}].back();
    int tradingday2 = mappp[{endyear, endmonth}].back();
    int firstday = mappp[{year, month}].front();

    double amount = initialAmount;
    string date = (tradingday < 10 ? "0" : "") + to_string(tradingday) + "-" +
                  (month < 10 ? "0" : "") + to_string(month) + "-" +
                  to_string(year);

    string firstdate = (firstday < 10 ? "0" : "") + to_string(firstday) + "-" +
                       (month < 10 ? "0" : "") + to_string(month) + "-" +
                       to_string(year);

    string date2 = to_string(tradingday2) + "-" +
                   (endmonth < 10 ? "0" : "") + to_string(endmonth) + "-" +
                   to_string(endyear);

    string filename = "portfolio_returns_" + firstdate + "-" + to_string(numOfStocks) + "-" + date2 + ".csv";
    string filenameforpowerbi="portfolio_returns.csv";

    int start = 0;
    for (int k = 0; start < portfolioreturns.size(); ++k) {
        auto entry = portfolioreturns[k];
        if (entry.first == date)
            break;
        else
            start++;
    }

    int end = 0;
    for (int k = 0; end < portfolioreturns.size(); ++k) {
        auto entry = portfolioreturns[k];
        if (entry.first == date2)
            break;
        else
            end++;
    }
    if (start > end) {
        cout << "Please enter the start date less than the end..." << endl;
        return;
    }

    cout<<"enter 1 to create a differint csv file\nenter 2 to update portfolio_returns file for powerbi"<<endl;
    int n=-1;
    cin>>n;
    if(n!=1&&n!=2)
    {
    cout<<"incorrect input"<<endl;
    return;
    }

    else if(n==1)
    {
    ofstream outFile(filename);
    outFile << "Your initial investment is: " << initialAmount << ",,,\n";
    outFile << "Date,Return (%),Portfolio Value\n";

    for (int j = start; j < end; ++j) {
        auto entry = portfolioreturns[j];
        double returnPercentage = entry.second;
        amount += (amount * returnPercentage / 100.0);
        outFile << entry.first << "," 
                << returnPercentage << "," 
                << amount << "\n";
    }
    outFile.close();

    cout << "CSV file '" << filename << "' created successfully." << endl;

    }

    else
    {
    ofstream outFile(filenameforpowerbi);
    outFile << "Your initial investment is: " << initialAmount << ",,,\n";
    outFile << "Date,Return (%),Portfolio Value\n";

    for (int j = start; j < end; ++j) {
        auto entry = portfolioreturns[j];
        double returnPercentage = entry.second;
        amount += (amount * returnPercentage / 100.0);
        outFile << entry.first << "," 
                << returnPercentage << "," 
                << amount << "\n";
    }
    outFile.close();

    cout << "CSV file '" << filenameforpowerbi << "' updated successfully." << endl;
    }
}

void readAndProcessFile(const string& filePath, vector<Stock>& stocks, int noofstocks, int noofdatapoints) {
    ifstream file(filePath);
    string line, date;

    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return;
    }

    getline(file, line);
    stringstream headerStream(line);
    vector<string> headers;
    string header;
    size_t stockCount = 0;

    while (getline(headerStream, header, ',') && stockCount < noofstocks) {
        headers.push_back(header);
        stockCount++;
    }

    vector<Stock> tempStocks;
    for (size_t i = 1; i < headers.size(); ++i) {
        tempStocks.emplace_back(headers[i]);
    }

    size_t rowCount = 0;
    while (getline(file, line) && rowCount < noofdatapoints) {
        stringstream ss(line);
        if (getline(ss, date, ',')) {
            for (size_t i = 1; i < headers.size(); ++i) {
                string priceStr;
                if (getline(ss, priceStr, ',')) {
                    priceStr.erase(remove(priceStr.begin(), priceStr.end(), ' '), priceStr.end());
                    if (!priceStr.empty() && isdigit(priceStr[0])) {
                        try {
                            double price = stod(priceStr);
                            tempStocks[i - 1].addPrice(date, price);
                        } catch (const exception& e) {
                            cerr << "Error converting price for date: " << date << " - " << e.what() << endl;
                        }
                    } else {
                        cerr << "Invalid price or empty line for date: " << date << endl;
                    }
                } else {
                    cerr << "Error extracting price for date: " << date << endl;
                }
            }
            rowCount++;
        } else {
            cerr << "Error extracting date from line: " << line << endl;
        }
    }

    for (size_t i = 0; i < tempStocks.size(); ++i) {
       cout << "Merging stock " << i << endl;
       int a= stocks[i].merge(tempStocks[i]); 
       if(a==-1)
       {
        cout<<"problem with stock: "<<tempStocks[i].name<<endl;
       }
    }
    file.close();
}





void readAndProcessFile2(const string& filePath, vector<Stock>& stocks, int noofstocks, int noofdatapoints, size_t startRow) {
    ifstream file(filePath);
    string line, date;

    if (!file.is_open()) {
        return;
    }
    getline(file, line);
    stringstream headerStream(line);
    vector<string> headers;
    string header;
    size_t stockCount = 0;

    while (getline(headerStream, header, ',') && stockCount < noofstocks) {
        headers.push_back(header);
        stockCount++;
    }

    stocks.clear();
    for (size_t i = 1; i < headers.size(); ++i) {
        stocks.emplace_back(headers[i]);
    }

    size_t rowCount = 0;
    while (rowCount < startRow && getline(file, line)) {
        rowCount++;
    }
    while (getline(file, line) && rowCount < startRow + noofdatapoints) {
        stringstream ss(line);
        
        if (getline(ss, date, ',')) {
            for (size_t i = 1; i < headers.size(); ++i) {
                string volumeStr;
                if (getline(ss, volumeStr, ',')) {
                    volumeStr.erase(remove(volumeStr.begin(), volumeStr.end(), ' '), volumeStr.end());

                    long long volume = 0;
                    if (!volumeStr.empty()) {
                        try {
                            volume = stoll(volumeStr);
                        } catch (const exception&) {
                        
                        }
                    }
                    stocks[i - 1].addVolume(date, volume);
                }
            }
            rowCount++;
        }
    }

    file.close();
}



map<int,vector<double>> fun7(vector<pair<string, double>> portfolioreturns)
{
    map<int,vector<double>> ans;
    for(auto it:portfolioreturns)
    {
        string date=it.first;
        double returns=it.second;
        int year=findyear(date);
        ans[year].pb(returns);
    }
    return ans;
}


map<int, double> calculateYearlyMDD(const map<int, vector<double>>& yearlyReturns) {
    map<int, double> yearlyMDD;
    for (const auto& entry : yearlyReturns) {
        int year = entry.first;
        double peak = -99999.0; 
        double currentMDD = 0.0; 
        const vector<double>& monthlyReturns = entry.second;
        for (size_t i = 0; i < monthlyReturns.size(); i++) {
            double NAV = monthlyReturns[i];
            if (NAV > peak) {
                peak = NAV;
            }
            double DD =  100*(peak - NAV) / peak;
            if (DD > currentMDD) {
                currentMDD = DD;
            }
        }
        yearlyMDD[year] = currentMDD;
    }

    return yearlyMDD;
}



int main() {
  fast_io;
    vector<Stock> stocks;
   
    string filePath = "E:/Desktop/algotrading/Nifty 500 v1 Price.csv";
    string filePath2="E:/Desktop/algotrading/stock_volumes_transformed_updated_500.csv";

     int noofstock;
    int noofdatapoints;
    cout<<"Enter no of Stocks you have in your csv file:"<<endl;
     cin>>noofstock;
    cout<<"Enter no of Data points you have :"<<endl;
    cin>>noofdatapoints;
     cout<<"reading data from csv file..."<<endl;

    cout<<"enter start row for voulme data"<<endl;

    size_t start_row;
    cin>>start_row;
     future<void> fileProcessingFuture = async(launch::async, readAndProcessFile2, filePath2, ref(stocks),noofstock+1,noofdatapoints,start_row);
    fileProcessingFuture.get();
   


    future<void> fileProcessingFuture2 = async(launch::async, readAndProcessFile, filePath, ref(stocks),noofstock+1,noofdatapoints);
    fileProcessingFuture2.get(); 
    
    
    int period;
    int athwindow;
    cout << "Enter the period of  EMA:" << endl;
    cin >> period;
    cout<<"Enter the window size for ATH"<<endl;
    cin>>athwindow;



    vector<future<void>> futures;
    for (auto& stock : stocks) {
        futures.push_back(async(launch::async, [&stock, period,athwindow]() {
            stock.calculateEMAForStock(period);
            stock.calculateATHForStock(athwindow);
            stock.return1Month();
            stock.return12months();
            stock.testfun();

        }));
    }
    for (auto& fut : futures) {
        fut.get();
    }
cout<<"calculations completed"<<endl;
        
   vector<vector<pair<double,pair<string,string>>>> datafor12months;
   vector<vector<pair<pair<double,double>,pair<string,string>>>> datafor1month;
    for(auto it: stocks)
    {
        vector<pair<double,pair<string,string>>> temp=monthreturns12forastock(it);
         vector<pair<pair<double,double>,pair<string,string>>> temp2=monthreturns1forastock(it);
        datafor12months.pb(temp);
        datafor1month.pb(temp2);
    }

   vector<pair<string, double>> p = stocks[0].prices;
   map<string, int> dateToIndexMap;
    vector<string> dates;  
    for (int i = 0; i < p.size(); ++i) {
        string date = convertToDDMMYYYY(p[i].first);
        dateToIndexMap[date] = i;
        dates.pb(p[i].first);
    }
   map<pair<int,int>,vector<int>> mappp = createDateMap(dates);
   map<string,vector<pair<double,string>>> datewisedatafor12monthsreturns;
   map<string,vector<pair<pair<double,double>,string>>> datewisedatafor1monthsreturns;
   map<string,vector<pair<double,string>>> sorteddatewisedatafor12monthsreturns;
   map<string,vector<pair<bool,string>>> emacheck;

    datewisedatafor12monthsreturns=fun1(datafor12months);
    datewisedatafor1monthsreturns=fun5(datafor1month);

    sorteddatewisedatafor12monthsreturns=fun2(datewisedatafor12monthsreturns);
      int percent;
        cout<<"enter how much percent of top stocks do you want:"<<endl;
        cin>>percent;
         
        map<string, vector<pair<double, string>>> topperccentstocksdatewise;
        topperccentstocksdatewise = fun3(percent, sorteddatewisedatafor12monthsreturns);
        int criteriaforath;
        cout<<"enter the criteria for ath:"<<endl;
        cin>>criteriaforath;
        emacheck = fun4(topperccentstocksdatewise, stocks,criteriaforath); 
       
int entryy = 0; 
while (entryy != 2) {
    cout << "Press 1 and enter to create a portfolio of stocks:" << endl
         << "Press 2 and enter to exit" << endl;

    cin >> entryy;
         
    switch (entryy) {
        case 1: {
            int noofstocks;
            cout << "Enter the number of Stocks you want in your Portfolio:" << endl;
            cin >> noofstocks;
            cout<<"please enter the start date:"<<endl;
            int month,year;
           cout<<"enter start year"<<endl;
            cin>>year;
            cout<<"enter month"<<endl;
            cin>>month;
            int endmonth,endyear;
            cout<<"enter end year:"<<endl;
            cin>>endyear;
            cout<<"enter end month:"<<endl;
            cin>>endmonth;
            if (noofstocks <= 0) {
                cout << "Invalid number of stocks. Please enter a positive integer." << endl;
                break;
            }
            vector<pair<string, double>> portfolioreturns = fun6(emacheck, datewisedatafor1monthsreturns, noofstocks, mappp);
            map<int,vector<double>> mapp=fun7(portfolioreturns);

            map<int,double> avarageannualrateofreturn;
            for (auto entry: mapp)
            {
                int year=entry.first;
                vector<double> returnss=entry.second;
                double sum=accumulate(all(returnss),0);
                double avg=sum/returnss.size();
                avarageannualrateofreturn[year]=avg;
            }

            double initialAmount;
            cout << "Enter how much money you want to invest: " << endl;
            cin >> initialAmount;

            if (initialAmount <= 0) {
                cout << "Invalid investment amount. Please enter a positive number." << endl;
                break;
            }
            cout << "Your initial investment money is: " << initialAmount << endl << endl;
        map<int,vector<double>> mapppp;
           mapppp= printPortfolioReturns(portfolioreturns, initialAmount,month,year,endmonth,endyear,mappp);
           cout<<endl<<endl;
           
           map<int, double> MDDResults = calculateYearlyMDD(mapppp);
             for(auto it: MDDResults){
                cout<<"YEAR: "<<it.first<<" MDD: "<<it.second<<endl;
                    }
                    cout<<endl<<endl;
                    map<int,double> calmerratio;
                    for(auto entry:MDDResults)
                    {
                        int year=entry.first;
                        double mdd=entry.second;
                        double avgreturns=avarageannualrateofreturn[year];
                        if(mdd==0)
                        {
                            calmerratio[year]=INT_MAX;
                            continue;
                        }
                        double ratio=avgreturns/mdd;
                        calmerratio[year]=ratio;
                    }

                     for(auto it: calmerratio){
                cout<<"YEAR: "<<it.first<<" Calmer Ratio: "<<it.second<<endl;
                    }
                    
               cout<<"do you want to create a csv file ? write 'yes' to create 'no' to not.... "<<endl;
            string cond;
            cin>>cond;

              if(toLower(cond)=="yes")
            {
          string filepath = "E:/Desktop/algotrading/portfolios";
          writePortfolioReturnsToCSV(portfolioreturns, initialAmount, month, year, endmonth, endyear, mappp, noofstocks);
            }
            break;
        }
        case 2: {
            cout << "Exiting...." << endl;
            break;
        }
        default: {
            cout << "Invalid input. Please enter 1 or 2." << endl;
            break;
        }
    }
}
    return 0;
}

