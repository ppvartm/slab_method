#include <map>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <ctime>

using namespace std;

class cell;
class point_for_cell;

class point
{
protected:
    double x;
    double y;
public:
    friend ostream& operator<<(ostream& out, point& p)
    {
        out << "( " << p.x << " ; " << p.y << " )";
        return out;
    }
    friend ifstream& operator>>(ifstream& fin, point& p)
    {
        fin >> p.x >> p.y;
        return fin;
    }

    double get_y() const
    {
        return y;
    }

    double get_x() const
    {
        return x;
    }
};

class otrezok
{
private:
    int n;
    double k;
    double b;
    bool f;
public:
    otrezok() :f(false) {};
    otrezok(int n_, double k_, double b_) :n(n_), k(k_), b(b_), f(false) {};
    double gt_x(double y)
    {
        return (y - b) / k;
    }
    void true_()
    {
        f = true;
    }
    int get_num() const
    {
        return n;
    }
    double get_k() const
    {
        return k;
    }
    friend point_for_cell;
};

class point_for_cell :public point
{
private:
    otrezok* otr1;
    otrezok* otr2;
public:
    point_for_cell() :otr1(nullptr), otr2(nullptr) {};
    point& operator=(const point& p)
    {
        x = p.get_x();
        y = p.get_y();
        return *this;
    }
    bool check_otr1()
    {
        return otr1->f;
    }
    bool check_otr2()
    {
        return otr2->f;
    }
    otrezok* get_otr1()
    {
        return otr1;
    }
    otrezok* get_otr2()
    {
        return otr2;
    }
    ~point_for_cell() { delete otr2; };
    friend cell;
};

class cell
{
private:
    int Num;
    int m;
    point_for_cell* pfc;
public:
    cell() :Num(0), m(0), pfc(nullptr) {};
    point_for_cell& operator[](int i)
    {
        if (i == m) return pfc[0];
        else if (i == -1) return pfc[m - 1];
        else return pfc[i];
    }
    void init(int num_, int m_)
    {
        Num = num_;
        m = m_;
        pfc = new point_for_cell[m_];
    }
    void create_otr()
    {
        for (int i = 0; i < m - 1; ++i)
        {
            if ((pfc[i + 1].x - pfc[i].x) != 0) {
                pfc[i].otr2 = new otrezok(Num, (pfc[i + 1].y - pfc[i].y) / (pfc[i + 1].x - pfc[i].x), pfc[i].y -
                    ((pfc[i + 1].y - pfc[i].y) / (pfc[i + 1].x - pfc[i].x)) * pfc[i].x);
                pfc[i + 1].otr1 = pfc[i].otr2;
            }
            else
            {
                pfc[i].otr2 = new otrezok(Num, -1000000000., 1000000000. * pfc[i].x);
                pfc[i + 1].otr1 = pfc[i].otr2;
            }
        }
        if ((pfc[0].x - pfc[m - 1].x) != 0) {
            pfc[m - 1].otr2 = new otrezok(Num, (pfc[0].y - pfc[m - 1].y) / (pfc[0].x - pfc[m - 1].x), pfc[m - 1].y -
                ((pfc[0].y - pfc[m - 1].y) / (pfc[0].x - pfc[m - 1].x)) * pfc[m - 1].x);
            pfc[0].otr1 = pfc[m - 1].otr2;
        }
        else
        {
            pfc[m - 1].otr2 = new otrezok(Num, -1000000000., 1000000000. * pfc[m - 1].x);
            pfc[0].otr1 = pfc[m - 1].otr2;
        }
    }
    ~cell() { delete[] pfc; }
};







int main() {


    ifstream fin("C:\\Users\\јртем\\Desktop\\problem.txt"); //помен€й путь на свой
    int n;
    fin >> n;
    vector<point> vec1(n);
    for (int i = 0; i < n; ++i)
        fin >> vec1[i];
    int p;
    fin >> p;
    vector<cell> vec2(p);
    map<double, point*> PFL_map;           //мэп дл€ полос
    multimap<double, point_for_cell*> PFC_map;  //мэп дл€ точек событий
    int a; int b;
    bool f;
    for (int i = 0; i < p; ++i)
    {
        fin >> a;
        vec2[i].init(i + 1, a);
        for (int j = 0; j < a; ++j)
        {
            fin >> b;
            vec2[i][j] = vec1[b - 1];
            PFL_map.emplace(vec1[b - 1].get_y(), &vec1[b - 1]);
        }
        vec2[i].create_otr();
        for (int k = 0; k < a; ++k)
            PFC_map.emplace(vec2[i][k].get_y(), &vec2[i][k]);
    }

    //считываение точек дл€ поиска из файла
    int enum_point_for_search;
    fin >> enum_point_for_search;
    vector<point> point_for_search(enum_point_for_search);
    for (int i = 0; i < enum_point_for_search; ++i)
        fin >> point_for_search[i];
    //---------------------------------
    clock_t tStart1 = clock();
    //определение полос, в которых лежат точки
    multimap<int, point*> map_for_search_point;                        //ключ-номер полосы, значение точка дл€ поиска в ней
    vector<int> num_of_slubs_with_point(enum_point_for_search);        //номер полос с точками (так, как они в файле)
    vector<int> sort_num_of_slubs_with_point(enum_point_for_search);   //номер полос с точками (отсортированный)
    map<double, int> slab_numbers;   //ключ - игрик полосы, значение - ее номер
    int k = 0;
    for (auto& it : PFL_map)
        slab_numbers.emplace(it.first, k++);
    for (int i = 0; i < enum_point_for_search; ++i)
    {
        auto it = slab_numbers.emplace(point_for_search[i].get_y(), 0);
        ++it.first;
        map_for_search_point.emplace(it.first->second, &point_for_search[i]);
        num_of_slubs_with_point[i] = it.first->second;     // номера полос, соответствующие пор€дку точек в файле
        sort_num_of_slubs_with_point[i] = it.first->second;
        --it.first;
        if (it.first->second == 0)
            slab_numbers.erase(point_for_search[i].get_y());
    }
    clock_t tFinish1 = clock();
    sort(sort_num_of_slubs_with_point.begin(), sort_num_of_slubs_with_point.end());
    //------------------------------------------------------
    cout << "time = " << ((tFinish1 - tStart1)) / 1000 << endl;
    //вектор с игриками всех полос по возрастанию
    vector<double> vec_line(PFL_map.size());
    int ii = 0;
    for (auto& p : PFL_map)
        vec_line[ii++] = p.first;
    //-----------------------------------------------------

    multimap<double, otrezok*> last_map;
    otrezok fict;

    map<int, point*> num_point_cell;
    //проход по деревь€м!
    vector<multimap<double, otrezok*>> vec_mp(sort_num_of_slubs_with_point[enum_point_for_search - 1]);
    int schet = 0;
    double x = 0;

    double t1 = clock();
    while (schet < sort_num_of_slubs_with_point[enum_point_for_search - 1])
    {
        auto it = PFC_map.equal_range(vec_line[schet]);
        for (auto itr = it.first; itr != it.second; ++itr)
        {
            if (itr->second->check_otr1() && !itr->second->check_otr2())
            {
                vec_mp[schet - 1].erase(itr->second->get_otr1()->gt_x((vec_line[schet] + vec_line[schet - 1]) / 2));
                vec_mp[schet].emplace(itr->second->get_otr2()->gt_x((vec_line[schet + 1] + vec_line[schet]) / 2),
                    itr->second->get_otr2());
                itr->second->get_otr2()->true_();
            }
            if (itr->second->check_otr2() && !itr->second->check_otr1())
            {
                vec_mp[schet - 1].erase(itr->second->get_otr2()->gt_x((vec_line[schet] + vec_line[schet - 1]) / 2));
                vec_mp[schet].emplace(itr->second->get_otr1()->gt_x((vec_line[schet + 1] + vec_line[schet]) / 2),
                    itr->second->get_otr1());
                itr->second->get_otr1()->true_();
            }
            if (itr->second->check_otr2() && itr->second->check_otr1())
            {
                vec_mp[schet - 1].erase(itr->second->get_otr1()->gt_x((vec_line[schet] + vec_line[schet - 1]) / 2));
                vec_mp[schet - 1].erase(itr->second->get_otr2()->gt_x((vec_line[schet] + vec_line[schet - 1]) / 2));
            }
            if (!itr->second->check_otr2() && !itr->second->check_otr1())
            {
                vec_mp[schet].emplace(itr->second->get_otr2()->gt_x((vec_line[schet + 1] + vec_line[schet]) / 2),
                    itr->second->get_otr2());
                vec_mp[schet].emplace(itr->second->get_otr1()->gt_x((vec_line[schet + 1] + vec_line[schet]) / 2),
                    itr->second->get_otr1());

                if ((itr->second->get_otr1()->get_k() == 0) && (itr->second->get_otr2()->get_k() != 0))
                    itr->second->get_otr2()->true_(); else
                    if ((itr->second->get_otr2()->get_k() == 0) && (itr->second->get_otr1()->get_k() != 0))
                        itr->second->get_otr1()->true_(); else
                    {
                        itr->second->get_otr2()->true_();
                        itr->second->get_otr1()->true_();
                    }
            }
        }

        if (schet != 0)
        {
            for (auto& itrmap : vec_mp[schet - 1])
                vec_mp[schet].emplace(itrmap.second->gt_x((vec_line[schet + 1] + vec_line[schet]) / 2), itrmap.second);
        }


        for (int i = 0; i < sort_num_of_slubs_with_point.size(); ++i)
        {
            if (schet + 1 == sort_num_of_slubs_with_point[i]) {
                clock_t tStart2 = clock();
                for (auto itertt = map_for_search_point.equal_range(schet + 1).first; itertt != map_for_search_point.equal_range(schet + 1).second; ++itertt)
                {
                    for (auto it = vec_mp[schet].begin(); it != vec_mp[schet].end(); ++it)
                    {
                        if (it->second->get_k() != 0)
                            last_map.emplace(it->second->gt_x(itertt->second->get_y()), it->second);
                    }
                    last_map.emplace(last_map.begin()->first - 1, &fict);
                    last_map.emplace(last_map.rbegin()->first + 1, &fict);
                    auto it = last_map.emplace(itertt->second->get_x(), &fict);
                    ++it;
                    auto right_it = last_map.equal_range(it->first);
                    auto rrr = right_it.second;
                    ----it;
                    auto left_it = last_map.equal_range(it->first);
                    auto lll = left_it.first;
                    --lll;
                    for (auto r = right_it.first; r != rrr; ++r)
                        for (auto l = lll; l != left_it.second; ++l)
                            if (r->second->get_num() == l->second->get_num())
                                num_point_cell.emplace(l->second->get_num(), itertt->second);
                   //   cout << r->second->get_num() << endl;}
                    last_map.clear();
                    clock_t tFinish2 = clock();
                    x += (double)((tFinish2 - tStart2));
                    //  cout << x / 1000 << endl;
                }


            }
        }


        ++schet;
    }


  //  double t2 = clock();


    for (int i = 0; i < point_for_search.size(); ++i)
         for (auto& it : num_point_cell)
             if (it.second == &point_for_search[i]) cout << it.first << endl;

   // cout << "time = " << (double)((tFinish1 - tStart1) + x) / CLOCKS_PER_SEC << endl;

    // cout << setprecision(20) << "=======================" <<endl<< (double)((tFinish1 - tStart1) + x) << endl;

    // cout << vec_mp[schet - 1].equal_range(10).first->second->get_num() << endl;

     /*auto it = PFC_map.equal_range(vec_line[schet]);
     for (auto itr = it.first; itr != it.second; ++itr)
         cout << itr->second->get_x() << "   ";
     cout << endl;*/
     // cout << schet << endl;


   //  for (auto& it : vec_line)
     //     cout << it<< endl;
}