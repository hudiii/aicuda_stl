#include <aicuda_stl_map.h>
#include <aicuda_stl_vector.h>
#include <aicuda_stl_string.h>
#include <aicuda_stl_list.h>
#include <aicuda_stl_set.h>
#include <stdio.h>

__global__ void
vectorAdd()
{
    printf("vectorAdd()\n");
    aicuda::stl::map<int, int> mp1;
    mp1[1] = 1;
    mp1[2] = 2;
    mp1[3] = 3;
    for(aicuda::stl::map<int, int>::const_iterator itr = mp1.begin();
        itr != mp1.end(); ++itr)
    {
        printf("kernel [ %d, %d ]\n", itr->first, itr->second);
    }

    aicuda::stl::vector<int> vt;
    aicuda::stl::string a("111");
    a = "222";
    printf("a = %s \n", a.c_str());

    vt.push_back(111);
    vt.push_back(222);

    aicuda::stl::map<int, aicuda::stl::vector<int> > mp2;
    mp2[1].push_back(1);
    mp2[1].push_back(11);
    mp2[1].push_back(111);
    mp2[2].push_back(2);
    mp2[2].push_back(22);
    mp2[2].push_back(222);
    for(aicuda::stl::map<int, aicuda::stl::vector<int> >::const_iterator itr = mp2.begin();
        itr != mp2.end(); ++itr)
    {
        printf("kernel [ %d = ", itr->first);
        size_t vt_count = itr->second.size();
        if(vt_count > 0)
        {
            printf("%d",itr->second[0]);
            for(size_t i = 1; i < vt_count; ++i)
            {
                printf(", %d",itr->second[i]); 
            }
            printf(" ]\n");
        }
        else
        {
            printf(" null ]\n");
        }
    }

    aicuda::stl::set<int> st;
    st.insert(1);
    st.insert(2);
    st.insert(1);
    st.insert(2);
    printf("st size = %ld \n", st.size());

    aicuda::stl::list<int> lt;
    lt.push_back(1);
    lt.push_back(2);
    lt.push_back(3);
    lt.push_front(5);

    lt.remove(3);

    for(aicuda::stl::list<int>::const_iterator itr = lt.begin();
        itr != lt.end(); ++itr)
    {
        printf("lt = %d\n", *itr);
    }

    printf("to_string(123) = %s\n", aicuda::stl::string::to_string(123).c_str());
    printf("to_string(\"123\") = %s\n", aicuda::stl::string::to_string("123").c_str());
    //printf("to_string(\"lt\") = %s\n", aicuda::stl::string::to_string(lt).c_str());
    printf("to_string(123.0) = %s\n", aicuda::stl::string::to_string(123.0).c_str());
    printf("to_string(123.0001) = %s\n", aicuda::stl::string::to_string(123.0001).c_str());
    printf("to_string(false) = %s\n", aicuda::stl::string::to_string(false).c_str());
    printf("to_string(true) = %s\n", aicuda::stl::string::to_string(true).c_str());


}

int main()
{
    printf("hello word\n");

    cudaError_t cudaStatus = cudaDeviceSetLimit(cudaLimitStackSize, 200 * 1024);
    if (cudaStatus != cudaSuccess) {
        printf("cudaDeviceSetLimit returned error after launching Kernel : %s!\n", cudaGetErrorString(cudaStatus));
        return -1;
    }

    vectorAdd<<<1, 1>>>();
    cudaStatus = cudaThreadSynchronize();
    if (cudaStatus != cudaSuccess) {
        printf("cudaDeviceSynchronize returned error after launching Kernel : %s!\n", cudaGetErrorString(cudaStatus));
    }

    cudaDeviceReset();
}