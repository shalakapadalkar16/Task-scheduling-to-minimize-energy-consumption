//task graph array size N needs to be changed before running for 20 tasks
#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>
#include <list>
#include <ctime>
#define N 20

using namespace std;

void primary_assignment(int t_g[][3], int A[], int t_res, int n, int k)
{
    int t_l_min[n];
    for(int i=0; i<n; i++)
    {
        t_l_min[i] = 0;
    }
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<k; j++)
        {
            if(t_l_min[i]>t_g[i][j])
            {
                t_l_min[i] = t_g[i][j];
            }
        }
    }
    //classify tasks: value of A[i] 1:local 0:cloud
    for(int i=0; i<n; i++)
    {
        if(t_l_min[i] > t_res)
        {
            A[i] = 1;
        }
        else
        {
            A[i] = 0;
        }
    }
}

void task_prioritizing(int t_g[][3], int pr[], int pr_n[],int n, int adj_mat[][N], int w[], int A[], int t_res, int k)
{
     for(int i=0; i<n; i++)
     {
        if(A[i] == 1)
        {
            w[i] = t_res;
        }
        else
        {
            int sum = 0;
            for(int j=0; j<k; j++)
            {
                sum += t_g[i][j];
            }
            w[i] = sum/k;
        }
    }
    //calculate average of t_g of i

    pr[n-1] = w[n-1];
    for(int i=n-1; i>=0; i--)
    {
        int max_j = 0;
        for(int j=n-1; j>=0; j--)
        {
            if(adj_mat[i][j] == 1 && pr[j] > max_j)
            {
                max_j = pr[j];
            }
        }
        pr[i] = w[i] + max_j;
    }
    //calculate priority

    vector<pair<int,int>> vectr;
    for (int i=0; i<n; i++)
    {
        vectr.push_back(make_pair(pr[i],i));
    }
    sort(vectr.begin(), vectr.end());
    for(int i=0; i<n; i++)
    {
        pr_n[i] = vectr[i].second;
    }
    //sort according to priority

}

void exec_unit_selection(int t_g[][3], int pr_n[], int n, int adj_mat[][N], int A[], int rt_l[], int rt_c[], int rt_ws[], int ft_ws[], int ft_wr[], int ft_l[], int ft[], int core[], int core1[], int k, int t_s, int t_r, int t_c, vector<vector<int>> cloud_list)
{
    int f_i = pr_n[n-1];//schedule first element
    rt_l[f_i] = 0;
    rt_ws[f_i] = 0;
    ft_ws[f_i] = rt_ws[f_i] + t_s;
    rt_c[f_i] = ft_ws[f_i];
    if(A[f_i] == 1)
    {
        ft_wr[f_i] = rt_c[f_i] + t_c + t_r;
        ft_l[f_i] = 0;
        ft[f_i] = ft_wr[f_i];
        core[3] = ft[f_i];
        cloud_list[0].push_back(f_i);
        core1[f_i] = 3;
    }
    else
    {
        int t_l_min = INT_MAX;
        int index;
        for(int i=0; i<k; i++)
        {
            if(t_g[f_i][i]<t_l_min)
            {
                t_l_min = t_g[f_i][i];
                index = i;
            }
        }
        ft_l[f_i] = rt_l[f_i] + t_l_min;
        ft_wr[f_i] = rt_c[f_i] + t_c + t_r;
        if(ft_l[f_i] <= ft_wr[f_i])
        {
            ft[f_i] = ft_l[f_i];
            ft_wr[f_i] = 0;
            core[3] = ft_ws[f_i];
            cloud_list[index+1].push_back(f_i);
            core1[f_i] = index;
        }
        else
        {
            ft[f_i] = ft_wr[f_i];
            ft_l[f_i] = 0;
            core[index] = ft[f_i];
            cloud_list[0].push_back(f_i);
            core1[f_i] = 3;
        }
    }
    //schedule rest of the elements
    for(int a=n-2; a>=0; a--)
    {
        int i = pr_n[a];
        //cout<<i;
        int max_j_l = 0;
        for(int j=0; j<n; j++)
        {
            if(adj_mat[j][i] == 1 && max_j_l < max(ft_l[j],ft_wr[j]))
            {
                max_j_l = max(ft_l[j],ft_wr[j]);
            }
        }
        rt_l[i] = max_j_l;
        int max_j_ws = 0;
        for(int j=0; j<n; j++)
        {
            if(adj_mat[j][i] == 1 && max_j_ws < max(ft_l[j],ft_ws[j]))
            {
                max_j_ws = max(ft_l[j],ft_ws[j]);
            }
        }
        rt_ws[i] = max_j_ws;
        ft_ws[i] = max(core[3],rt_ws[i]) + t_s;
        int max_j_c = 0;
        for(int j=0; j<n; j++)
        {
            if(adj_mat[j][i] == 1 && max_j_c < ft_wr[j]-t_r)
            {
                max_j_c = ft_wr[j]-t_r;
            }
        }
        rt_c[i] = max(ft_ws[i],max_j_c);
        if(A[i] == 1)
        {
            ft_wr[i] = rt_c[i] + t_c + t_r;
            ft[i] = ft_wr[i];
            ft_l[i] = 0;
            core[3] = ft_ws[i];
            cloud_list[0].push_back(i);
            core1[i] = 3;
        }
        else
        {
            int rt, index;
            int f = INT_MAX;
            for(int j=0; j<k; j++)
            {
                rt = max(rt_l[i],core[j]);
                if(f > rt + t_g[i][j])
                {
                    f = rt + t_g[i][j];
                    index = j;
                }
            }
            rt_l[i] = f - t_g[i][index];
            ft_l[i] = f;
            ft_wr[i] = rt_c[i] + t_c + t_r;
            if(ft_l[i] <= ft_wr[i])
            {
                ft[i] = ft_l[i];
                ft_wr[i] = 0;
                core[index] = ft[i];
                cloud_list[index+1].push_back(i);
                core1[i] = index;
            }
            else
            {
                ft[i] = ft_wr[i];
                ft_l[i] = 0;
                core[3] = ft[i];
                cloud_list[0].push_back(i);
                core1[i] = 3;
            }
        }
    }
}

void kernel_algo(vector<vector<int>> cloud_list, int t_g[][3],int n, int adj_mat[][N],int core1[], int tmax, int t_total, float e_total, int k, int st[], int et[], int E_c, int E_l[][3])
{
    int out = 0;
    int count = 0;
    while(out == 0)
    {
        float max_ratio = 0;
        int new_n = 0, new_k = 0, new_index1 = 0, new_index2 = 0, new_t = t_total;
        float new_e = e_total;
        int less_t1 =0, less_t2 = 0;
        int temp_core[20], new_st[20], new_ft[20];
        for(int i=0; i<n; i++)
        {
            for(int j=0; j<k+1; j++)
            {
                int core2[20], core3[4], rt[20], rt1[20], ft[20], ft1[20], pushed[20];
                vector<vector<int>> time_list(4);
                int index1, index2 = 0;
                for(int i=0; i<n; i++)
                {
                    rt[i] = 0;
                    ft[i] = 0;
                    core2[i] = core1[i];
                    ft1[i] = et[i];
                    rt1[i] = st[i];
                    pushed[i] = 0;
                }
                for(int a=0; a<cloud_list.size(); a++)
                {
                    //core2[i] = core1[i];
                    core3[a] = 0;
                    for(int b=0; b<cloud_list[a].size(); b++)
                    {
                        time_list[a].push_back(cloud_list[a][b]);
                    }
                }
                int current_core = core1[i];
                for(int a=0; a<time_list[current_core].size(); a++)
                {
                    if(time_list[current_core][a] == i)
                    {
                        index1 = a;
                    }
                }
                time_list[current_core].erase(time_list[current_core].begin()+index1);
                //calculate the ready time of target task
                if(j == 3)
                {
                    int max_j_ws = 0;
                    for(int a=0; a<n; a++)
                    {
                        if(adj_mat[a][i] == 1 && max_j_ws < ft1[a])
                        {
                            max_j_ws = ft1[a];
                        }
                    }
                    rt[i] = max_j_ws;
                }
                else
                {
                    int max_j_l = 0;
                    for(int a=0; a<n; a++)
                    {
                        if(adj_mat[a][i] == 1 && max_j_l < ft1[a])
                        {
                            max_j_l = ft1[a];
                        }
                    }
                    rt[i] = max_j_l;
                }
                core2[i] = j;

                if(time_list[j].size() == 0)
                {
                    index2 = 0;
                }
                else if(time_list[j].size() == 1)
                {
                    if(rt1[time_list[j][0]] > rt[i])
                    {
                        index2 = 0;
                    }
                    else
                    {
                        index2 = 1;
                    }
                }
                else
                {
                    if(rt1[time_list[j][0]] > rt[i])
                    {
                        index2 = 0;
                    }
                    else if(rt1[time_list[j][time_list[j].size()-1]] <= rt[i])
                    {
                        index2 = time_list[j].size();
                    }
                    else
                    {
                        for(int b=0; b<time_list[j].size()-1; b++)
                        {
                            if(rt[i]>=rt1[time_list[j][b]] && rt[i]<=rt1[time_list[j][b+1]])
                            {
                                index2 = b+1;
                            }
                        }
                    }
                }
                time_list[j].insert(time_list[j].begin()+index2,i);

                // initialize ready1 and ready2
                int ready1[20], ready2[20];
                for(int a=0; a<n; a++)
                {
                    ready1[a] = 0;
                }
                for(int a=0; a<n; a++)
                {
                    for(int b=0; b<n; b++)
                    {
                        if(adj_mat[a][b] == 1)
                        {
                            ready1[b] += 1;
                        }
                    }
                    ready2[a] = 1;
                }

                for(int a=0; a<4; a++)
                {
                    if(time_list[a].size()>0)
                    {
                        ready2[time_list[a][0]] = 0;
                    }
                }

                //initialize the stack and implement the first operation
                stack<int> S;
                for(int a=0; a<n; a++)
                {
                    if(ready1[a] == 0 && ready2[a] == 0 && pushed[a] == 0)
                    {
                        S.push(a);
                        pushed[a] = 1;
                        ;
                    }
                }

                int current1 = S.top();
                S.pop();
                rt[current1] = 0;
                if(core2[current1] == 3)
                {
                    rt[current1] = max(core3[core2[current1]],rt[current1]);
                    ft[current1] = rt[current1] + 5;
                    core3[core2[current1]] = rt[current1] + 3;
                }
                else
                {
                    rt[current1] = max(core3[core2[current1]],rt[current1]);
                    ft[current1] = rt[current1] + t_g[current1][core2[current1]];
                    core3[core2[current1]] = ft[current1];
                }

                for(int a=0; a<n; a++)
                {
                    if(adj_mat[current1][a] == 1)
                    {
                        ready1[a] -= 1;
                    }
                }
                ready2[current1] = 1;

                if(time_list[core2[current1]].size()>1)
                {
                    for(int a=1; a<time_list[core2[current1]].size(); a++)
                    {
                        if(time_list[core2[current1]][a-1] == current1)
                        {
                            ready2[time_list[core2[current1]][a]] = 0;
                        }
                    }
                }

                for(int a=0; a<n; a++)
                {
                    if(ready1[a] == 0 && ready2[a] == 0 && pushed[a] == 0)
                    {
                        S.push(a);
                        pushed[a] = 1;
                    }
                }

                while(S.size() != 0)
                {
                    int current = S.top();
                    S.pop();

                    //ca/culate ready time of current task
                    if(core2[current] == 3)
                    {
                        int max_j_ws1 = 0;
                        for(int a=0; a<n; a++)
                        {
                            if(adj_mat[a][current] == 1 && max_j_ws1 < ft[a])
                            {
                                max_j_ws1 = ft[a];
                            }
                        }
                        rt[current] = max_j_ws1;
                    }
                    else
                    {
                        int max_j_l1 = 0;
                        for(int a=0; a<n; a++)
                        {
                            if(adj_mat[a][current] == 1 && max_j_l1 < ft[a])
                            {
                                max_j_l1 = ft[a];
                            }
                        }
                        rt[current] = max_j_l1;
                    }
                    if(core2[current] == 3)
                    {
                        rt[current] = max(core3[core2[current]],rt[current]);
                        ft[current] = rt[current] + 5;
                        core3[core2[current]] = rt[current] + 3;
                    }
                    else
                    {
                        rt[current] = max(core3[core2[current]],rt[current]);
                        ft[current] = rt[current] + t_g[current][core2[current]];
                        core3[core2[current]] = ft[current];
                    }
                    //update ready1 and ready2
                    for(int a=0; a<n; a++)
                    {
                        if(adj_mat[current][a] == 1)
                        {
                            ready1[a] -= 1;
                        }
                    }
                    ready2[current] = 1;
                    if(time_list[core2[current]].size()>1)
                    {
                        for(int a=1; a<time_list[core2[current]].size(); a++)
                        {
                            if(time_list[core2[current]][a-1] == current)
                            {
                                ready2[time_list[core2[current]][a]] = 0;
                            }
                        }
                    }
                    for(int a=0; a<n; a++)
                    {
                        if(ready1[a] == 0 && ready2[a] == 0 && pushed[a] == 0)
                        {
                            S.push(a);
                            pushed[a] = 1;

                        }
                    }
                }

                int curr_t = ft[n-1];
                int curr_e = 0;
                for(int a=0; a<n; a++)
                {
                    if(core2[a] == 3)
                    {
                        curr_e += E_c;
                    }
                    else
                    {
                        curr_e += E_l[a][core2[a]];
                    }
                }

                if(curr_t <= t_total && curr_e < new_e)
                {
                    less_t1 = 1;
                    new_n = i;
                    new_k = j;
                    new_index1 = index1;
                    new_index2 = index2;
                    new_t = curr_t;
                    new_e = curr_e;

                    for(int a=0; a<n; a++)
                    {
                        temp_core[a] = core2[a];
                        new_st[a] = rt[a];
                        new_ft[a] = ft[a];
                    }
                }
                if(curr_t > t_total && curr_t <= tmax && less_t1 == 0 && curr_e < e_total && max_ratio < double((e_total - curr_e) / (curr_t - t_total)))
                {
                    max_ratio = double((e_total - curr_e) / (curr_t - t_total));

                    less_t2 = 1;
                    new_n = i;
                    new_k = j;
                    new_index1 = index1;
                    new_index2 = index2;
                    new_t = curr_t;
                    new_e = curr_e;

                    for(int a=0; a<n; a++)
                    {
                        temp_core[a] = core2[a];
                        new_st[a] = rt[a];
                        new_ft[a] = ft[a];
                    }
                }
            }
        }
        if(less_t1 != 1 && less_t2 != 1)
        {
            out = 1;
        }
        else
        {
            cloud_list[core1[new_n]].erase(cloud_list[core1[new_n]].begin()+new_index1);
            cloud_list[new_k].insert(cloud_list[new_k].begin()+new_index2,new_n);
            t_total = new_t;
            e_total = new_e;
            for(int a=0; a<n; a++)
            {
                core1[a] = temp_core[a];
                st[a] = new_st[a];
                et[a] = new_ft[a];
            }
            if(less_t1 != 1 && less_t2 != 1)
            {
                out = 1;
            }
            count += 1;
            cout<<"Step "<<count<<endl;
            cout<<"Current Operation: Insert Task "<<new_n+1<<" to Core "<<new_k<<endl;
            cout<<"Current Completion Time: "<<t_total<< "   Current Energy Consumption: "<<e_total<<endl;
        }
    }

    cout << "-------------------------------------------------------------------------------------------------------------";

    cout<<endl;

    cout<<"Best Energy Consumption: "<<e_total<<"   Best Completion Time: "<<t_total<<endl;

    cout<<"Final Task Scheduling"<<endl;

    for(int i=0; i<cloud_list.size(); i++)
    {
        if(i == 3)
        {
            cout<<"Wireless sending : ";
            for(int j=0; j<cloud_list[i].size(); j++)
            {
                cout<<st[cloud_list[i][j]]<<"-Task"<<cloud_list[i][j]+1 <<"--"<< et[cloud_list[i][j]]  - 2<<" ";
            }
            cout<<endl;
            cout << endl;

            cout<<"Cloud: ";
            for(int j=0; j<cloud_list[i].size(); j++)
            {
                cout<<st[cloud_list[i][j]] +3<<"--Task"<<cloud_list[i][j] + 1 <<"--"<<et[cloud_list[i][j]] - 1<<" ";
            }
            cout<<endl;
            cout<<endl;

            cout<<"Wireless receiving : ";
            for(int j=0; j<cloud_list[i].size(); j++)
            {
                cout<<st[cloud_list[i][j]] + 4<<"--Task"<<cloud_list[i][j] + 1 <<"--"<<et[cloud_list[i][j]]<<" ";
            }
            cout<<endl;
            cout <<endl;
        }
        else
        {
            cout<<"Core"<<i+1<<": ";
            for(int j=0; j<cloud_list[i].size(); j++)
            {
            cout<<st[cloud_list[i][j]]<<"--Task"<<cloud_list[i][j] + 1<<"--"<<et[cloud_list[i][j]]<<" ";
            }
        }
        cout << endl;
        cout << endl;

    }
    cout<<"----------------------------------------------------------------------------------------------------------"<<endl;
    }

int main()
{
/*int t_g[][3]={{9,7,5},
              {8,6,5},
              {6,5,4},
              {7,5,3},
              {5,4,2},
              {7,6,4},
              {8,5,3},
              {6,4,2},
              {5,3,2},
              {7,4,2}};*/

/*int adj_mat[][10]={ {0,1,1,1,1,1,0,0,0,0}, //adjacency matrix of the task graph
                    {0,0,0,0,0,0,0,1,1,0},
                    {0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,0,1,1,0},
                    {0,0,0,0,0,0,0,0,1,0},
                    {0,0,0,0,0,0,0,1,0,0},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,0}};*/

/*int adj_mat[][10]={ {0,1,1,1,1,1,0,0,0,0},
                    {0,0,0,0,0,0,0,1,0,0},
                    {0,0,0,0,0,0,1,1,1,0},
                    {0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,1,1,0,0},
                    {0,0,0,0,0,0,0,0,1,0},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,0}};*/

int t_g[][3]={{9,7,5},
              {8,6,5},
              {6,5,4},
              {7,5,3},
              {5,4,2},
              {7,6,4},
              {8,5,3},
              {6,4,2},
              {5,3,2},
              {7,4,2},
              {6,5,2},
              {8,7,6},
              {9,4,2},
              {7,5,2},
              {6,5,3},
              {8,6,2},
              {7,5,4},
              {5,4,3},
              {8,4,3},
              {7,6,2}};

/*int adj_mat[][20]={  {0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //example 3
                    {0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};*/


/*int adj_mat[][20]={ {0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //example 4
                    {0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};*/

int adj_mat[][20]={ {0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //example 5
                    {0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};


    int n = N;
    int t_max;
    if(n == 10)
    {
        t_max = 27;
    }
    else if(n == 20)
    {
        t_max = 46.5;
    } // t_max = 27 for 10 tasks and 46.5 for 20 tasks

    cout << "Number of tasks: "<<n<<endl;
    cout << endl;

    int A[20]; //local:0, cloud:1
    int pr[20], pr_n[20]; //priority
    int w[20], core[4], core1[20];
    int rt_l[20], rt_c[20], rt_ws[20], ft_ws[20], ft_wr[20], ft_l[20], ft[20];
    vector<vector<int>> cloud_list(4);


    for(int i=0; i<n; i++)
    {
        A[i] = 0;
        pr[i] = 0;
        pr_n[i] = 0;
        w[i] = 0;
        rt_l[i] = 0;
        rt_ws[i] = 0;
        rt_c[i] = 0;
        ft_ws[i] = 0;
        ft_wr[i] = 0;
        ft_l[i] = 0;
        ft[i] = 0;
        core1[i] = 0;
    }
    for(int i=0; i<4; i++)
    {
        core[i] = 0;
    }
    int k = 3;
    int t_s = 3, t_c = 1, t_r = 1;
    int t_res = t_s + t_c + t_r;
    int E_l[20][3];
    int p_c[] = {1,2,4};
    float ps = 0.5;
    float E_c = ps * t_s;
    for(int i=0; i<n; i++)
    {
      for(int j=0; j<3; j++)
      {
          E_l[i][j] = p_c[j] * t_g[i][j];
      }
    }
    //auto start = chrono::high_resolution_clock::now();
    clock_t time_req1;
    clock_t time_req2;
    time_req1 = clock();
    time_req2 = clock();

    primary_assignment(t_g,A,t_res,n,k);
    task_prioritizing(t_g,pr,pr_n,n,adj_mat,w,A,t_res,k);

    exec_unit_selection(t_g,pr_n,n,adj_mat,A,rt_l,rt_c,rt_ws,ft_ws,ft_wr,ft_l,ft,core,core1,k,t_s,t_r,t_c,cloud_list);

    time_req2 = clock() - time_req2;

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<n; j++)
        {
            if(core1[j] == i)
            {
                cloud_list[i].push_back(j);
            }
        }
    }
    float e_total = 0;
    for(int i=0; i<n; i++)
    {
        if(core1[i] == 3)
        {
            e_total += E_c;
        }
        else
        {
            e_total += E_l[i][core1[i]];
        }
    }
    int st[20];
    for(int i=0; i<n; i++)
    {
        st[i] = max(rt_l[i],rt_ws[i]);
        //cout<<st[i]<<endl;
        //cout<<core1[i]<<" ";
    }

    int t_min = ft[n-1];


    cout<<"Initial Scheduling"<<endl;

    for(int i=0; i<cloud_list.size(); i++)
    {
        if(i == 3)
        {
            cout<<"Wireless sending : ";
            for(int j=0; j<cloud_list[i].size(); j++)
            {
                cout<<st[cloud_list[i][j]]<<"-Task"<<cloud_list[i][j]+1 <<"--"<< ft[cloud_list[i][j]]  - 2<<" ";
            }
            cout<<endl;
            cout <<endl;

            cout<<"Cloud: ";
            for(int j=0; j<cloud_list[i].size(); j++)
            {
                cout<<st[cloud_list[i][j]] +3<<"--Task"<<cloud_list[i][j] + 1 <<"--"<<ft[cloud_list[i][j]] - 1<<" ";
            }
            cout<<endl;
            cout << endl;

            cout<<"Wireless receiving : ";
            for(int j=0; j<cloud_list[i].size(); j++)
            {
                cout<<st[cloud_list[i][j]] + 4<<"--Task"<<cloud_list[i][j] + 1 <<"--"<<ft[cloud_list[i][j]]<<" ";
            }
            cout<<endl;
        }
        else
        {
            cout<<"Core"<<i+1<<": ";
            for(int j=0; j<cloud_list[i].size(); j++)
            {
                cout<<st[cloud_list[i][j]]<<"--Task"<<cloud_list[i][j] + 1<<"--"<<ft[cloud_list[i][j]]<<" ";
            }
        }
        cout<<endl;
        cout<<endl;
    }
    cout<<"Initial Energy Consumption: "<<e_total<<"   Initial Completion Time: "<<t_min<<endl;
    cout<<endl;
    cout<<"----------------------------------------------------------------------------------------------------------"<<endl;;
    kernel_algo(cloud_list,t_g,n,adj_mat,core1,t_max,t_min,e_total,k,st,ft,E_c,E_l);
    time_req1 = clock() - time_req1;

    cout << endl << "Time taken by scheduling result after task migration is : "<<(double)time_req1 / CLOCKS_PER_SEC<<"seconds"<< endl;

    system("pause");
    return 0;
}