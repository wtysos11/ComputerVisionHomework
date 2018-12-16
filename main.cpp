#include "CImg.h"
//#include "otsu.h"
#include "meanShift.h"
#include <iostream>
using namespace cimg_library;
using namespace std;


int main(void)
{
    /*
    otsu o("test11.bmp");
    int threshold = o.compute();
    cout<<threshold<<endl;
    o.outputBio(threshold);
*/
/*
    meanShift m("test21.bmp");
    m.compute();
*/
    vector<vector<double>> from,to;
    from.push_back(vector<double>{38671803.6437,2578831.9242});
    from.push_back(vector<double>{38407102.8445, 2504239.2774});
    from.push_back(vector<double>{38122268.3963, 2358570.38514});
    from.push_back(vector<double>{38126455.4595, 2346827.2602});
    from.push_back(vector<double>{38177232.2601, 2398763.77833});
    from.push_back(vector<double>{38423567.3485, 2571733.9203});
    from.push_back(vector<double>{38636876.4495, 2543442.3694});
    from.push_back(vector<double>{38754169.8762, 2662401.86536});
    from.push_back(vector<double>{38410773.8815, 2558886.6518});
    from.push_back(vector<double>{38668962.0430, 2578747.6349});

    to.push_back(vector<double>{38671804.6165, 2578831.1944});
    to.push_back(vector<double>{38407104.0875, 2504239.1898});
    to.push_back(vector<double>{38122269.2925, 2358571.57626});
    to.push_back(vector<double>{38126456.5675, 2346826.27022});
    to.push_back(vector<double>{38177232.3973, 2398762.11714});
    to.push_back(vector<double>{38423565.7744, 2571735.2278});
    to.push_back(vector<double>{38636873.6217, 2543440.7216});
    to.push_back(vector<double>{38754168.8662, 2662401.86101});
    to.push_back(vector<double>{38410774.5621, 2558886.0921});
    to.push_back(vector<double>{38668962.5493, 2578746.94});

    vector<double> m(affine_fit(from,to));
    if(m.size()==0)
    {
        cout<<"failed"<<endl;
    }
    else
    {
        for(int i = 0;i<m.size();i++)
        {
            cout<<m[i]<<endl;
        }
    }
    return 0;
}
