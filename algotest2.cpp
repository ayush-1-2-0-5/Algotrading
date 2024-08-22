#include <bits/stdc++.h>
#include <thread>
#include <future>
#include <cmath>
#include <iomanip>
#include <ctime>
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

//class statrs
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

    bool isFirstOfMonth(const string& date) {
        return date.substr(0,2) == "01";
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



public:
    string name;
    vector<pair<string, double>> prices; 
    vector<pair<string, double>> ema;
    vector<pair<string, double>> ath;   
    vector<pair<string, bool>> isgood;
    vector<pair<string, double>> returns12M;    
    vector<pair<string,double>>monthwisereturnsforstock;
     vector<pair<string,pair<double, double>>> returns1M;
     vector<pair<string,pair<double,double>>>monthwisereturns1Mforstock;

    Stock(const string& stockName) : name(stockName) {}
    void addPrice(const string& date, double price) {
        prices.pb(make_pair(date, price));
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
        vector<double> athValues = calculateATH(closePrices, period);
        for (size_t i = 0; i < athValues.size(); ++i) {
            ath.pb(make_pair(dates[i], athValues[i]));
        }
    }

    void IsEMAcriteria(double a) {
        for (size_t i = 0; i < prices.size(); ++i) {
            if (i <= ema.size() || i <= ath.size()) {
                isgood.pb(make_pair(prices[i].first, false));
                continue;
            }
            double currentPrice = prices[i].second;
            double currentEMA = ema[i].second;
            double currentATH = ath[i].second;
            bool conditionMet = (currentEMA < currentPrice) && (currentPrice < (a/100) * currentATH);
            isgood.pb(make_pair(prices[i].first, conditionMet));
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


string toLower(const std::string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

void readAndProcessFile(const string& filePath, vector<Stock>& stocks) {
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
    while (getline(headerStream, header, ',') && stockCount < 501) {
        headers.pb(header);
        stockCount++;
    }
    stocks.clear();
    for (size_t i = 1; i < headers.size(); ++i) {
        stocks.emplace_back(headers[i]);
    }
    size_t rowCount = 0;
    while (getline(file, line) && rowCount <= 2350) {
        stringstream ss(line);
        if (getline(ss, date, ',')) {
            for (size_t i = 1; i < headers.size(); ++i) {
                string priceStr;
                if (getline(ss, priceStr, ',')) {
                    priceStr.erase(remove(priceStr.begin(), priceStr.end(), ' '), priceStr.end());

                    if (!priceStr.empty() && isdigit(priceStr[0])) {
                        try {
                            double price = stod(priceStr);
                            stocks[i - 1].addPrice(date, price);
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
    file.close();
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

string convertToYYYYMMDD(const string& date) {
    stringstream ss(date);
    string day, month, year;
    getline(ss, day, '-');
    getline(ss, month, '-');
    getline(ss, year, '-');
    return year + "-" + month + "-" + day;
}
    
map<string, vector<pair<bool, string>>> fun4(map<string, vector<pair<double, string>>> &topperccentstocksdatewise, vector<Stock> stocks) {
    cout << "test line 1" << endl;
    map<string, vector<pair<bool, string>>> answer;
    map<string, int> dateToIndexMap;
    vector<pair<string, double>> p = stocks[0].prices;

    vector<string> dates;  
    for (int i = 0; i < p.size(); ++i) {
        string date = convertToDDMMYYYY(p[i].first);
        dateToIndexMap[date] = i;
        dates.pb(p[i].first);
    }
 
    map<pair<int,int>,vector<int>> mappp = createDateMap(dates);

    map<string, string> futuredates;

    for (auto& itr : topperccentstocksdatewise) {
        string currdate = itr.first;
        string newdate = find_futuredate(mappp, currdate);
        futuredates[currdate] = newdate;
    }

    int pointer = 0;
    vector<future<void>> futures;
    for (auto it : topperccentstocksdatewise) {
        futures.push_back(async(launch::async, [&answer, &dateToIndexMap, &stocks, it, mappp, futuredates]() {
            string date = it.first;
            string futureDate = futuredates.at(date);
            vector<pair<bool, string>> tmp;
            int j = -1;
            if (dateToIndexMap.find(convertToYYYYMMDD(futureDate)) != dateToIndexMap.end()) {
                j = dateToIndexMap[convertToYYYYMMDD(futureDate)];
            }
            if (j == -1) return;

            for (const auto& stockPair : it.second) {
                string stockName = stockPair.second;
                auto itr = find_if(stocks.begin(), stocks.end(), [&stockName](const Stock& stock) {
                    return stock.name == stockName;
                });

                if (itr != stocks.end()) {
                    auto prices = itr->prices;
                    auto ema = itr->ema;
                    if (prices[j].second > ema[j].second) {
                        tmp.push_back({true, stockName});
                    } else {
                        tmp.push_back({false, stockName});
                    }
                }
            }

            answer[date] = tmp;
        }));
    }
    for (auto& fut : futures) {
        fut.get();
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

string finddate2(string currdate, map<pair<int, int>, vector<int>> mappp) {
    int currYear, currMonth, currDay;
    parseDate(currdate, currYear, currMonth, currDay);
    int targetYear = currYear;
    int targetMonth = currMonth + 13;

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

map<string, pair<double, string>> fun6(map<string, vector<pair<bool, string>>> emacheck,map<string, vector<pair<pair<double,double>, string>>> datewisedatafor1monthsreturns,int noostocks,map<pair<int, int>, vector<int>> mappp) 
{
    map<string, pair<double, string>> answer;

    set<string> portfoliostocks;
    string bufferdate="";
   
       for(auto it:emacheck)
       {

        string backtestdate = it.first;
        vector<pair<bool, string>>& backtestdata = it.second;
        vector<pair<pair<double,double>, string>> futureData2;
        string future_date = finddate(backtestdate, mappp);
         
        // string future_date=finddate2(backtestdate,mappp);
        map<string,double> returnsmap;
        map<string,double> continuedreturnsmap;
        if (datewisedatafor1monthsreturns.find(convertToYYYYMMDD(future_date)) != datewisedatafor1monthsreturns.end()) {
           
            vector<pair<pair<double,double>, string>>& futureData = datewisedatafor1monthsreturns[convertToYYYYMMDD(future_date)];

            if(bufferdate.size())
             futureData2 = datewisedatafor1monthsreturns[convertToYYYYMMDD(bufferdate)];

            cout << "Future Date: " << convertToYYYYMMDD(future_date) << " sizeoffutureData " << futureData.size()<<endl;
            if(futureData2.size())
            cout << "Future Date2: " << convertToYYYYMMDD(bufferdate) << " sizeoffutureData " << futureData2.size()<<endl;

            cout<<endl;

            for (const auto& data : futureData) {
                returnsmap[data.second]=data.first.first;
                 
            }
            for(const auto&data:futureData2)
            {
              continuedreturnsmap[data.second]=data.first.second;
            }
        }
    if(portfoliostocks.empty())
         {
        auto itr=it.second;
        cout<<endl;
        for(auto i:itr)
        {
            string stock=i.second;
           
            bool c=i.first;
             double returnss=returnsmap[stock];
             if(c)
             {
            portfoliostocks.insert(stock);
            cout<<"stock: "<<stock<<" returns : "<< returnss<<endl;
             }
        
        }
         }

         else
         {
             auto itr=it.second;
             
             cout<<endl;
             for (auto i:itr)
             {
                 set<string> temp;

                string stock=i.second;
                bool c=i.first;
                temp.insert(stock);
                if(portfoliostocks.find(stock)!=portfoliostocks.end())
                {
                       double returnss=continuedreturnsmap[stock];
                       if(c)
                       {
                         cout<<"stock: "<<stock<<" returns : "<< returnss<<endl;
                       }
                }
                else
                {
                      double returnss=returnsmap[stock];
                          if(c)
                       {
                         cout<<"stock: "<<stock<<" returns : "<< returnss<<endl;
                       }
                }
                portfoliostocks=temp;

             }
         }

        bufferdate=future_date;
       }
        return answer;
    }
   










int main() {
  fast_io;
    vector<Stock> stocks;
    string filePath = "E:/Desktop/algotrading/Nifty 500 v1 Price.csv";
    future<void> fileProcessingFuture = async(launch::async, readAndProcessFile, filePath, ref(stocks));
    fileProcessingFuture.get();
    int period;
    double c;
    cout << "Enter the period of  EMA:" << endl;
    cin >> period;
    int ath_period;
    cout<<"Enter the period of  ATH:(around 250 for a year)" << endl;
    cin>>ath_period;
    cout << "Enter the criteria for ATH and EMA relation (no between 50-90):" << endl;
    cin >> c;
    vector<future<void>> futures;
    for (auto& stock : stocks) {
        futures.push_back(async(launch::async, [&stock, period,ath_period, c]() {
            stock.calculateEMAForStock(period);
            stock.calculateATHForStock(ath_period);
            stock.return1Month();
            stock.return12months();
            stock.IsEMAcriteria(c);
        }));
    }
    for (auto& fut : futures) {
        fut.get();
    }
    

    cout << "Enter the stock name:" << endl;
    string name;
    cin >> name;
    int tar = -1;
    for (int i = 0; i < stocks.size(); ++i) {
        if (toLower(stocks[i].name) == toLower(name)) {
            tar = i;
            break;
        }
    }
    cout << tar << endl;
    if (tar == -1) {
        cout << "Could not find the stock" << endl;
    } else {
        if (!stocks[tar].ema.empty() && !stocks[tar].ath.empty() && !stocks[tar].isgood.empty()) {
            cout << "EMA and ATH for stock " << stocks[tar].name << " for the period of " << period << " days and "<< ath_period<<" days respectivly are:" << endl;
            cout << setw(5) << "Row" << setw(12) << "Date" << setw(15) << "EMA" << setw(15) << "ATH" << setw(10) << "Is Good" << setw(40) << "12M returns as on 1st of each month" << endl;
            size_t maxSize = max({stocks[tar].ema.size(), stocks[tar].ath.size(), stocks[tar].isgood.size()});
            for (size_t i = 0; i < maxSize; ++i) {
                cout << setw(5) << i + 2 << setw(12);
                if (i < stocks[tar].ema.size()) {
                    cout << stocks[tar].ema[i].first;
                } else {
                    cout << " ";
                }
                cout << setw(15);
                if (i < stocks[tar].ema.size()) {
                    cout << stocks[tar].ema[i].second;
                } else {
                    cout << " ";
                }
                cout << setw(15);
                if (i < stocks[tar].ath.size()) {
                    cout << stocks[tar].ath[i].second;
                } else {
                    cout << " ";
                }
                cout << setw(10);
                if (i < stocks[tar].isgood.size()) {
                    cout << (stocks[tar].isgood[i].second ? "true" : "false");
                } else {
                    cout << " ";
                }
                cout << setw(40);
                if (i < stocks[tar].returns12M.size()) {
                    if (stocks[tar].returns12M[i].second != -1) {
                        cout << stocks[tar].returns12M[i].second * 100;
                    } else {
                        cout << "----";
                    }
                } else {
                    cout << " ";
                }
                cout << endl;
            }
        } else {
            cout << "No EMA, ATH, or Is Good data available." << endl;
        }

        cout<<endl;
   vector<vector<pair<double,pair<string,string>>>> datafor12months;
   vector<vector<pair<pair<double,double>,pair<string,string>>>> datafor1month;

    for(auto it: stocks)
    {
        vector<pair<double,pair<string,string>>> temp=monthreturns12forastock(it);
         vector<pair<pair<double,double>,pair<string,string>>> temp2=monthreturns1forastock(it);
        datafor12months.pb(temp);
        datafor1month.pb(temp2);
    }

    cout << "Enter the stock name:" << endl;
    string name;
    cin >> name;
    int target = -1;
    for (int i = 0; i < stocks.size(); ++i) {
        if (toLower(stocks[i].name) == toLower(name)) {
            target = i;
            break;
        }
    }
    cout << target << endl;
    if (target == -1) {
        cout << "Could not find the stock" << endl;
    } 
    else{
        cout<<"entred here"<<endl;
             for (auto &entry : datafor12months[target]) {
        cout << "Returns 12 M: " << entry.first*100 << " Date: " << entry.second.first << " Stock: " << entry.second.second << endl;
             }
          for (auto &entry : datafor1month[target]) {
        cout << "Returns 1 M on first : " << entry.first.first<<" Returns 1M ON LAST : "<<entry.first.second << " Date: " << entry.second.first << " Stock: " << entry.second.second << endl;
    }
    }

//mapping of dates
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
  for (const auto& entry : topperccentstocksdatewise) {
        cout << "Date: " << entry.first << "\n";
        for (int i = 0; i < entry.second.size(); ++i) {
            auto pair = entry.second[i];
            cout << "  Stock: " << setw(60) << left << pair.second 
                 << "Returns: " << pair.first << "\n";
        }
        cout << endl;
    }

    cout<<endl<<endl<<endl;
     emacheck = fun4(topperccentstocksdatewise, stocks);

      cout<<"entered into"<<endl;
    for (const auto& entry : emacheck) {
        cout << "Date: " << entry.first << endl;
        for (int i = 0; i < entry.second.size(); ++i) {
            auto pair = entry.second[i];
            cout << "  Stock: " << setw(60) << left << pair.second 
                 << "Decision: " << (pair.first ? "TRUE" : "FALSE") << "\n";
        }
        cout << endl;
    }
  
    // for (const auto& monthData : datafor1month) {
    //     cout << "Month Data:" << endl;
    //     for (const auto& entry : monthData) {
    //         double value = entry.first;
    //         string startDate = entry.second.first;
    //         string endDate = entry.second.second;

    //         cout << "Returns: " << value*100 << ", Start Date: " << startDate << ", stock: " << endDate << endl;
    //     }
    //     cout << endl; // Separate each month's data with a newline
    // }




//  for (const auto& entry : datewisedatafor1monthsreturns) {
//         const string& date = entry.first;
//         const vector<pair<double, string>>& returnsAndStocks = entry.second;
//         cout << "Date: " << date << endl;
//         for (const auto& pair : returnsAndStocks) {
//             double returns = pair.first;
//             const string& stock = pair.second;

//             cout << "  Returns: " << returns << ", Stock: " << stock << endl;
//         }
//         cout << endl; 
//     }


int noofstocks=30;


     map<string,pair<double,string>> tempdata=fun6(emacheck,datewisedatafor1monthsreturns,noofstocks,mappp);
     



     cout<<"exited into"<<endl;

    }
    return 0;
}

