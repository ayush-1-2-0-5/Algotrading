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
    for (int year = 2002; year <= 2024; ++year) {
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


double calculateStandardDeviation(std::vector<double>& dayWiseReturns) {
    if (dayWiseReturns.empty()) return 0.0;
    double mean = accumulate(dayWiseReturns.begin(), dayWiseReturns.end(), 0.0) / dayWiseReturns.size();
    double sumSquaredDiffs = 0.0;
    for (const auto& value : dayWiseReturns) {
        double diff = value - mean;
        sumSquaredDiffs += diff * diff;
    }
    return sqrt(sumSquaredDiffs / dayWiseReturns.size());
}

double findDayWiseReturns(vector<double>& arr) {
    vector<double> dayWiseReturns;
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        if (arr[i - 1] == 0) {
            dayWiseReturns.push_back(nan("")); 
            continue;
        }
        double r = (arr[i] - arr[i - 1]) / arr[i - 1];
        double pr = r * 100; 
        dayWiseReturns.push_back(pr);
    }
    return calculateStandardDeviation(dayWiseReturns);
}

vector<pair<string, double>> calculate12MStddev(const vector<string>& dates, const vector<double>& closePrices) {
   vector<pair<string, double>> answer;
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
        //   cout << "Year: " << year << ", Month: " << month << ", Day: " << day <<",price: "<< closePrices[i]<< endl;
        dataMap[{year, month}].emplace_back(day, closePrices[i]);
    }


// cout<<endl<<endl<<endl;
    for (int year = 2002; year <= 2024; ++year) {
        for (int month = 1; month <= 12; ++month) {
            vector<double> temp;
            auto currYearData = dataMap.find({year, month});
            if (currYearData != dataMap.end()) {
                auto firstTradingDayCurrent = currYearData->second.front();
                for (int i = 0; i < 12; ++i) {
                    int m = month + i;
                    int y=year+m/13;
                    m=m%12;
                    if(m==0)
                    {
                        m=12;
                    }
                    auto data = dataMap.find({y, m});
                    if (data != dataMap.end()) {
                        for (const auto& dayData : data->second) {
                            // cout << "Year: " << y << ", Month: " << m <<",prices: "<<dayData.second<< endl; 
                            if(dayData.second!=0)
                            temp.push_back(dayData.second);
                        }

                    }
                }
                // cout<<endl<<endl<<endl;
                double stdDeviation = findDayWiseReturns(temp);
                string currDate = to_string(year) + "-" + (month < 10 ? "0" : "") + to_string(month) + "-" + 
                                        (firstTradingDayCurrent.first < 10 ? "0" : "") + to_string(firstTradingDayCurrent.first);
                answer.emplace_back(currDate, stdDeviation);
            }
        }
    }
    return answer;
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
    for (int year = 2002; year <= 2024; ++year) {
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
    vector<pair<string,double>>monthwisestddevforstock;
    vector<pair<string,pair<double, double>>> returns1M;
    vector<pair<string,pair<double,double>>>monthwisereturns1Mforstock;
    vector<pair<string,double>> standarddeviation;
    vector<pair<string, double>> stddev12M;
    map<string,double> mapdatetoprice;


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

    void fillprice()
    {
        for(auto it:prices)
        {
          double r=it.second;
          string d=it.first;
          mapdatetoprice[d]=r;
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
    void IsEMAcriteria(double a) {
        for (size_t i = 0; i < prices.size(); ++i) {
            if (i <= ema.size() || i <= ath.size()) {
                isgood.pb(make_pair(prices[i].first, false));
                continue;
            }
            double currentPrice = prices[i].second;
            double currentEMA = ema[i].second;
            double currentATH = ath[i].second;
            bool conditionMet = (currentEMA < currentPrice) && (currentPrice >= (a/100) * currentATH);
            isgood.pb(make_pair(prices[i].first, conditionMet));
        }
    } 
    void standarddev()
   {
     vector<double> closePrices;
     
    vector<string> dates;
    for (const auto& entry : prices) {
        if(entry.second!=0.0)
        {
        dates.pb(entry.first);
        closePrices.pb(entry.second);
        }
    }

    vector<pair<string, double>> result = calculate12MStddev(dates,closePrices);
     monthwisestddevforstock=result;
    unordered_map<string, double> resultMap;
    for (const auto& entry : result) {
        string convertedDate = convertToDDMMYYYY(entry.first); 
        resultMap[convertedDate] = entry.second;
    }
    for (const auto& date : dates) {
        if (resultMap.find(date) != resultMap.end() && resultMap[date] != 0) {
            stddev12M.pb(make_pair(date, resultMap[date]));
        }
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
void readAndProcessFile(const string& filePath, vector<Stock>& stocks,int noofstocks,int noofdatapoints) {
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
        headers.pb(header);
        stockCount++;
    }
    stocks.clear();
    for (size_t i = 1; i < headers.size(); ++i) {
        stocks.emplace_back(headers[i]);
    }
    size_t rowCount = 0;
    while (getline(file, line) && rowCount <= noofdatapoints) {
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

vector<pair<double,pair<string,string>>> monthstddev12forastock(Stock stocks){
vector<pair<double,pair<string,string>>>ans ;
    string name=stocks.name;
    auto stddev12m=stocks.monthwisestddevforstock;
    for(auto it:stddev12m)
    {
        string date=it.first;
        double stddev=it.second;
        ans.pb({stddev,{date,name}});
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


map<string,vector<pair<double,string>>> fun1_1(vector<vector<pair<double,pair<string,string>>>> data){

    map<string,vector<pair<double,string>>> mpp;
       for(int i=0;i<data.size();++i)
       {
           for(int j=0;j<data[i].size();++j)
           {
              auto it=data[i][j];
              double stddev=it.first;
              string date=it.second.first;
              string name=it.second.second;
              mpp[date].pb({stddev,name});
           }
       }
       return mpp;
}


map<string, vector<pair<double, string>>> fun2(map<string, vector<pair<double, string>>> datewisedatafor12monthsreturns, map<string, vector<pair<double, string>>> datewisedatafor12monthsstddev) {
    map<string, vector<pair<double, string>>> mpp;

    for (const auto& entry : datewisedatafor12monthsreturns) {
        string date = entry.first;
        vector<pair<double, string>> returns_pairs = entry.second;
        vector<pair<double, string>> stddev_pairs = datewisedatafor12monthsstddev[date];
        unordered_map<string, double> stddev_map;
        for (const auto& stddev_pair : stddev_pairs) {
            stddev_map[stddev_pair.second] = stddev_pair.first;
        }
        vector<pair<double, string>> ratio_pairs;
        for (const auto& ret_pair : returns_pairs) {
            double return_value = ret_pair.first;
            string stock_name = ret_pair.second;

            if (stddev_map.find(stock_name) != stddev_map.end() && stddev_map[stock_name] != 0) {  // Ensure stddev is not 0
                double stddev_value = stddev_map[stock_name];
                double ratio = return_value / stddev_value;
                ratio_pairs.push_back({ratio, stock_name});
            }
        }

        // Sort based on the ratio
        sort(ratio_pairs.begin(), ratio_pairs.end(), [](const pair<double, string>& a, const pair<double, string>& b) {
            if (isnan(a.first)) return false;
            if (isnan(b.first)) return true;
            if (a.first == numeric_limits<double>::infinity()) return false;
            if (b.first == numeric_limits<double>::infinity()) return true;
            return a.first > b.first;  // Sort in descending order
        });

        if (!ratio_pairs.empty()) {
            mpp[date] = ratio_pairs;
        }
    }

    return mpp;
}

map<string, vector<pair<double, string>>> fun2_2(map<string, vector<pair<double, string>>> datewisedatafor12monthsreturns) {
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


int findFirstTradingDay(const vector<int>& days) {
    if (days.empty()) return -1;
    return days.front(); 
}

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


string convertToYYYYMMDD(const string& date) {
    stringstream ss(date);
    string day, month, year;
    getline(ss, day, '-');
    getline(ss, month, '-');
    getline(ss, year, '-');
    return year + "-" + month + "-" + day;
}
    
map<string, vector<pair<bool, string>>> fun4(map<string, vector<pair<double, string>>> &topperccentstocksdatewise, vector<Stock> stocks,int criteriaforath) {
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
        futures.push_back(async(launch::async, [&answer, &dateToIndexMap, &stocks, it, criteriaforath,mappp, futuredates]() {
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
                    auto ath=itr->ath;
                    bool cond=false;
                    cond=(prices[j].second>ema[j].second&&  prices[j].second>=ath[j].second*criteriaforath/100);

                    if (cond) {
                        tmp.push_back({cond, stockName});
                    } else {
                        tmp.push_back({cond, stockName});
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

void createcsvfun6(
    map<string, vector<pair<bool, string>>> emacheck,
    map<string, vector<pair<pair<double, double>, string>>> datewisedatafor1monthsreturns,
    int noofstocks,
    map<pair<int, int>, vector<int>> mappp,
    map<string,int> mapstockstoindex,
    vector<Stock> stocks) 
{
    ofstream csvFile("portfolio_return_overview.csv");
    if (!csvFile.is_open()) {
        throw runtime_error("Unable to open CSV file for writing");
    }

    set<string> portfoliostocks;
    string bufferdate = "";  
    vector<vector<string>> nameData;
    vector<vector<double>> returnData;
    vector<vector<string>> statusData;
    vector<double> avgreturnss;
    vector<vector<double>> initialprice;
    vector<vector<double>> finalprice;
    vector<string> dateeee;
    vector<string> dateeee2;

    for (const auto& it : emacheck) {
        const string& backtestdate = it.first;
        const vector<pair<bool, string>>& backtestdata = it.second;
        
        vector<string> names;
        vector<double> returns;
        vector<string> statuses;
        vector<double> ip;
        vector<double> fp;
        
        ip.reserve(noofstocks);
        fp.reserve(noofstocks);
        names.reserve(noofstocks);
        returns.reserve(noofstocks);
        statuses.reserve(noofstocks);
        
        string future_date = finddate(backtestdate, mappp);
        string future_date_str = convertToYYYYMMDD(future_date);
        
        map<string, double> returnsmap;
        map<string, double> continuedreturnsmap;
        
        if (datewisedatafor1monthsreturns.find(future_date_str) != datewisedatafor1monthsreturns.end()) {
            const auto& futureData = datewisedatafor1monthsreturns[future_date_str];
            for (const auto& data : futureData) {
                returnsmap[data.second] = data.first.first;
            }
            
            if (!bufferdate.empty()) {
                string continued_future_date_str = convertToYYYYMMDD(bufferdate);
                if (datewisedatafor1monthsreturns.find(continued_future_date_str) != datewisedatafor1monthsreturns.end()) {
                    const auto& futureData2 = datewisedatafor1monthsreturns[continued_future_date_str];
                    for (const auto& data : futureData2) {
                        continuedreturnsmap[data.second] = data.first.second;
                    }
                }
            }
        }

        int stocksAdded = 0;
        for (const auto& entry : backtestdata) {
            if (stocksAdded >= noofstocks) break;
            const string& stock = entry.second;
            int idx = mapstockstoindex[stock];
            // cout<<idx<<" ";
            map<string,double> mapdatetop = stocks[idx].mapdatetoprice;
            bool isSelected = entry.first;
            
            if (isSelected) {
                double returnss;
                string start_date = find_futuredate2(mappp, backtestdate);
                string end_date = find_lasttradingday(mappp, convertToYYYYMMDD(start_date));
                string start_date_c=find_futuredate(mappp, backtestdate);
                // cout<<start_date<<" "<<end_date<<endl;
                
                if (portfoliostocks.find(stock) != portfoliostocks.end()) {
                    auto it = continuedreturnsmap.find(stock);
                    if (it == continuedreturnsmap.end()) continue;
                    auto start_price_it = mapdatetop.find(start_date_c);
                    auto end_price_it = mapdatetop.find(end_date);
                    if (start_price_it != mapdatetop.end() && end_price_it != mapdatetop.end()) {
                        ip.push_back(start_price_it->second);
                        fp.push_back(end_price_it->second);
                    } else {
                        ip.push_back(0.0);
                        fp.push_back(0.0);
                    }
                    
                    returnss = it->second;
                    names.push_back(stock);
                    returns.push_back(returnss);
                    statuses.push_back("existing");
                } else {
                    auto it = returnsmap.find(stock);
                    if (it == returnsmap.end()) continue;
                    
                    auto start_price_it = mapdatetop.find(start_date);
                    auto end_price_it = mapdatetop.find(end_date);
                    
                    if (start_price_it != mapdatetop.end() && end_price_it != mapdatetop.end()) {
                        ip.push_back(start_price_it->second);
                        fp.push_back(end_price_it->second);
                    } else {
                        ip.push_back(0.0);
                        fp.push_back(0.0);
                    }
                    
                    returnss = it->second;
                    names.push_back(stock);
                    returns.push_back(returnss);
                    statuses.push_back("new");
                }
                stocksAdded++;
            }
        }
        cout<<endl;
        
        if (names.size() == noofstocks) {
            portfoliostocks.clear();
            portfoliostocks.insert(names.begin(), names.end());
            bufferdate = future_date;
            double sum = 0.0;
            for(auto r : returns) {
                sum += r;
            }
            
            avgreturnss.push_back(returns.empty() ? 0.0 : sum/returns.size());
            nameData.push_back(names);
            returnData.push_back(returns);
            statusData.push_back(statuses);
            initialprice.push_back(ip);
            finalprice.push_back(fp);
            dateeee.push_back(find_futuredate2(mappp, backtestdate));
            dateeee2.push_back(find_futuredate(mappp, backtestdate));
        }
    }

    if (nameData.empty() || returnData.empty() || statusData.empty()) {
        csvFile.close();
        throw runtime_error("No valid data to write to CSV");
    }

    // Write CSV headers
    for (size_t i = 0; i < dateeee.size(); ++i) {
        csvFile << "Name,Monthly returns,Status,Initial price,Final price,SL triggered,SL triggered date,,";
    }
    csvFile << "\n\n";

    // Write trading days information
    for (const auto& date : dateeee) {
        csvFile << "First trading Day," << date << ",,,,,,,";
    }
    csvFile << "\n";
    
    for (int i = 0; i < dateeee2.size()-1; ++i) {
        csvFile << "Last trading Day," << dateeee2[i+1] << ",,,,,,,";
    }
    csvFile << "\n";
    
    for (const auto& date : dateeee2) {
        csvFile << "Rollover trading Day," << date << ",,,,,,,";
    }
    csvFile << "\n";
    
    // Write monthly returns
    for (size_t i = 0; i < dateeee.size(); ++i) {
        csvFile << "Monthly Returns," << avgreturnss[i] << ",,,,,,,";
    }
    csvFile << "\n\n";

    // Write stock data
    for (int i = 0; i < noofstocks; ++i) {
        for (size_t j = 0; j < nameData.size(); ++j) {
            if (i < nameData[j].size() && i < returnData[j].size() && i < statusData[j].size()) {
                csvFile << nameData[j][i] << "," 
                       << returnData[j][i] << "," 
                       << statusData[j][i] << ","
                       << initialprice[j][i] << ","
                       << finalprice[j][i] << ",,,,";
            } else {
                csvFile << ",,,,,,,,";
            }
        }
        csvFile << "\n";
    }

    csvFile.flush();
    csvFile.close();
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
    cout<<tradingday<<endl;
    int tradingday2=mappp[{endyear,endmonth}].back();
    cout<<tradingday2<<endl;
    int firstday=mappp[{year,month}].front();
    map<int,vector<double>> answer;
            for(auto it: portfolioreturns)
            {
                cout<<it.first<<" "<<it.second<<endl;
            }
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


void writePortfolioReturnsToCSV(
    const std::vector<std::pair<std::string, double>>& portfolioreturns,
    double initialAmount,
    int month,
    int year,
    int endmonth,
    int endyear,
    std::map<std::pair<int, int>, std::vector<int>> mappp,
    int numOfStocks,
    std::map<int, std::pair<double, double>> mddandcalmer
) {
    int tradingday = mappp[{year, month}].back();
    int tradingday2 = mappp[{endyear, endmonth}].back();
    int firstday = mappp[{year, month}].front();

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
    string filenameforpowerbi = "portfolio_returns1.csv";
    int start = 0, end = 0;
    for (int k = 0; start < portfolioreturns.size(); ++k) {
        if (portfolioreturns[k].first == date) break;
        ++start;
    }
    for (int k = 0; end < portfolioreturns.size(); ++k) {
        if (portfolioreturns[k].first == date2) break;
        ++end;
    }

    if (start > end) {
        cout << "Please enter the start date less than the end..." << endl;
        return;
    }

    cout << "Enter 1 to create a different CSV file\nEnter 2 to update the portfolio_returns file for Power BI" << endl;
    int n = -1;
    cin >> n;
    if (n != 1 && n != 2) {
        cout << "Incorrect input" << endl;
        return;
    }

    ofstream outFile(n == 1 ? filename : filenameforpowerbi);
    outFile << "Initial Investment: " << initialAmount << ",,,,\n";

    outFile << "Date,Return (%),Portfolio Value,,Year, MDD, Calmer Ratio\n";
  
    double amount = initialAmount;
    for (int j = start; j < end; ++j) {
        auto entry = portfolioreturns[j];
        double returnPercentage = entry.second;
        amount += (amount * returnPercentage / 100.0);
        outFile << entry.first << ","
                << returnPercentage << ","
                << amount << ",,"; 

        auto mddEntry = mddandcalmer.find(year);
        if (mddEntry != mddandcalmer.end()) {
            outFile << mddEntry->first << ","
                    << mddEntry->second.first << ","
                    << mddEntry->second.second << "\n";
        } else {
            outFile << ",,\n";
        }

        year++;
    }

    outFile.close();
    cout << "CSV file '" << (n == 1 ? filename : filenameforpowerbi) << "' " 
         << (n == 1 ? "created" : "updated") << " successfully." << endl;
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

void printCalmarRatioTable(const map<int, double>& MDDResults, const map<int, double>& calmerRatio) {
    cout << setw(10) << "Year" 
         << setw(20) << "MDD" 
         << setw(35) << "Calmar Ratio" 
         << endl;
    cout << "-------------------------------------------------------------------------------" << endl;

    for (const auto& it : calmerRatio) {
        cout << setw(10) << it.first 
             << setw(20);
            cout << MDDResults.at(it.first)<<setw(35);
        if(it.second==INT_MAX)
        {
            cout<<"There Was No Dropdown in the year "<<it.first<<endl;
        }
        else
        {
          cout << it.second <<endl;
        }

    }
}

map<int,double> calculateAverage36MonthReturn(const map<int, vector<double>>& mapp) {
     map<int,double>m;
       vector<pair<int,double>>v;

     for(auto it:mapp){
        double x=0;
        int ct=0;
        for(auto i:it.second){
            x+=i;
            ct++;
        }
        if(ct==12)
        {
        v.push_back({it.first,x});
        m[it.first]=x;
        }
     }

     for(int i=1;i<v.size();i++)v[i].second+=v[i-1].second;
     v.insert(v.begin(),{0,0});
     map<int,double>haha;
     for(int i=3;i<v.size();i++){
        double pre=v[i].second-v[i-3].second;
        pre/=(36);
        haha[v[i].first]=pre;
     }


    return haha;
}
void printSortedData(const map<string, vector<pair<double, string>>>& sortedData) {
    for (const auto& dateEntry : sortedData) {
        const string& date = dateEntry.first;
        const vector<pair<double, string>>& stockData = dateEntry.second;
 cout<<endl<<endl;
        cout << "Date: " << date << endl;
        for (const auto& stockEntry : stockData) {
            double returns = stockEntry.first;
            const string& stockName = stockEntry.second;

            cout << "Stock Name: " << stockName << ", Returns: " << returns << endl;
        }
        
        cout << "---------------------------" << endl<<endl;
    }
}

map<string,int> mapstocktoindex(vector<Stock> stocks)
{
  map<string,int> mp;
  for(int i=0;i<stocks.size();++i)
  {
      mp[stocks[i].name]=i;
  }
   return mp;
}

int main() {
  fast_io;
    vector<Stock> stocks;
    string filePath = "E:/Desktop/algotrading/prices_nifty_500.csv";
    int noofstock;
    int noofdatapoints;
    cout<<"Enter no of Stocks you have in your csv file:"<<endl;
     cin>>noofstock;
    cout<<"Enter no of Data points you have :"<<endl;
    cin>>noofdatapoints;
     cout<<"reading data from csv file..."<<endl;
    future<void> fileProcessingFuture = async(launch::async, readAndProcessFile, filePath, ref(stocks),noofstock+1,noofdatapoints);
    fileProcessingFuture.get();
    int period;
    int athwindow;
    for(auto it:stocks)
    {
        cout<<it.name <<" "<< it.prices.size()<<endl;
    }
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
            stock.standarddev();
            stock.fillprice();
        }));
    }
    for (auto& fut : futures) {
        fut.get();
    }
    Stock st=stocks[0];
    auto mapdatetoprice=st.mapdatetoprice;
    auto mapstockstoindex=mapstocktoindex(stocks);
   vector<vector<pair<double,pair<string,string>>>> datafor12months;
   vector<vector<pair<double,pair<string,string>>>> datafor12monthsstddev;
   vector<vector<pair<pair<double,double>,pair<string,string>>>> datafor1month;
    for(auto it: stocks)
    {
        vector<pair<double,pair<string,string>>> temp=monthreturns12forastock(it);
        vector<pair<pair<double,double>,pair<string,string>>> temp2=monthreturns1forastock(it);
        vector<pair<double,pair<string,string>>> temp3=monthstddev12forastock(it);
        datafor12months.pb(temp);
        datafor1month.pb(temp2);
        datafor12monthsstddev.pb(temp3);
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
   map<string,vector<pair<double,string>>> datewisedatafor12monthsstddev;
   map<string,vector<pair<double,string>>> sorteddatewisedatafor12monthsreturns;
   map<string,vector<pair<bool,string>>> emacheck;
    datewisedatafor12monthsreturns=fun1(datafor12months);
     datewisedatafor12monthsstddev=fun1_1(datafor12monthsstddev);
    datewisedatafor1monthsreturns=fun5(datafor1month);
    cout<<"enter how you want to sort: \n1)Enter 1 for sorting on the basis of 12M returns of a Stock\n2) Enter 2 for sorting on the basis of 12M/stddev for each stock "<<endl;

    int cho=-1;
   while(cho != 1 && cho != 2) 
{
    cin >> cho;
    if (cho != 1 && cho != 2)  
    {
        cout << "Please Enter 1 or 2 as choice" << endl;
    }
}
if(cho==2)
{
    sorteddatewisedatafor12monthsreturns=fun2(datewisedatafor12monthsreturns,datewisedatafor12monthsstddev);
}

else if(cho==1)
{
    sorteddatewisedatafor12monthsreturns=fun2_2(datewisedatafor12monthsreturns);
}


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
             createcsvfun6(emacheck, datewisedatafor1monthsreturns, noofstocks, mappp,mapstockstoindex,stocks);
             cout<<"hello"<<endl<<endl;
            vector<pair<string, double>> portfolioreturns = fun6(emacheck, datewisedatafor1monthsreturns, noofstocks, mappp);
              map<int,vector<double>> mapp=fun7(portfolioreturns);
  map<int, double> averageAnnualRateOfReturn;
  averageAnnualRateOfReturn=  calculateAverage36MonthReturn(mapp);

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
                    map<int,double> calmerratio;
                    for(auto entry:MDDResults)
                    {
                        int year=entry.first;
                        double mdd=entry.second;
                        double avgreturns=averageAnnualRateOfReturn[year];
                        if(mdd==0)
                        {
                            calmerratio[year]=INT_MAX;
                            continue;
                        }
                        double ratio=avgreturns/mdd*12;
                        if(ratio!=0.0)
                        calmerratio[year]=ratio;
                    }

            printCalmarRatioTable(MDDResults,calmerratio);

            map<int,pair<double,double>> mddandcalmer;
            for(auto it: MDDResults)
            {
                int year=it.first;
                double mdd=it.second;
                double calmer=calmerratio[year];
                if(calmer==0)
                {
                    continue;
                }
                if(mdd==double(0))
                {
                    continue;
                }
                mddandcalmer[year]=make_pair(mdd,calmer);
            }
               cout<<"do you want to create a csv file ? write 'yes' to create 'no' to not.... "<<endl;
            string cond;
            cin>>cond;
              if(toLower(cond)=="yes")
            {
          string filepath = "E:/Desktop/algotrading/portfolios";
          writePortfolioReturnsToCSV(portfolioreturns, initialAmount, month, year, endmonth, endyear, mappp, noofstocks,mddandcalmer);
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
