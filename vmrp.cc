/*
 * Filename:	vmrp.h
 * copyright:	2016 chenxin
 * Function:	the program used to simulate the algorithm of virtual memory page replacement
 */
#include "vmrp.h"

Replace::Replace() {
    int i;
    cout << "Please input page numbers:";
    //设定总的访问页数,并分配相应的引用页号和淘汰页号记录数组空间
    cin >> PageNumber;
    ReferencePage = new int[sizeof(int)*PageNumber];
    EliminatePage = new int[sizeof(int)*PageNumber];
    //输入引用页号序列(页面走向),初始化引用页数组
    cout << "Please input reference page string:";
    for(i=0; i<PageNumber; i++)
	cin >> ReferencePage[i]; //引用页暂存引用数组
    //设定内存实页数(帧数),并分配相应的实页号记录数组空间(页号栈)
    cout << "Please input page frames:";
    cin >> FrameNumber;
    PageFrames = new int[sizeof(int)*FrameNumber];
}

Replace::~Replace() {
    delete[] ReferencePage;
    delete[] EliminatePage;
    delete[] PageFrames;
}

void Replace::InitSpace(const char* MethodName)
{
    int i;
    cout << endl << MethodName << endl;
    FaultNumber = 0;
    //引用还未开始,-1 表示无引用页
    for(i=0; i<PageNumber; i++)
	EliminatePage[i] = -1;
    for(i=0; i<FrameNumber; i++)
	PageFrames[i] = -1;
}
//分析统计选择的算法对于当前输入的页面走向的性能
void Replace::Report(void) {
    //报告淘汰页顺序
    cout << endl << "Eliminate page:";
    for(int i=0; EliminatePage[i]!=-1;i++)
	cout << EliminatePage[i] << ' ';
    //报告缺页数和缺页率
    cout << endl << "Number of page faults = " << FaultNumber << endl;
    cout << setw(6) << setprecision(3);
    cout << "Rate of page faults = " << 100*(float)FaultNumber/(float)PageNumber << "%" << endl;
}
//最近最旧未用置换算法
void Replace::Lru(void)
{
    int i,j,k,l,next;
    InitSpace("LRU");
    //循环装入引用页
    for(k=0,l=0; k < PageNumber; k++) {
	next = ReferencePage[k];
	//检测引用页当前是否已在实存
	for(i=0; i<FrameNumber; i++) {
	    if(next == PageFrames[i]) {
		//引用页已在实存将其调整到页记录栈顶
		for(j=i;j>0;j--)
		    PageFrames[j] = PageFrames[j-1];
		PageFrames[0] = next;
		break;
	    }
	}
	if(PageFrames[0] == next) {
	    //如果引用页已放栈顶,则为不缺页,报告当前内存页号
	    for(j=0; j<FrameNumber; j++)
		if(PageFrames[j]>=0)
		    cout << PageFrames[j] << " ";
	    cout << endl;
	    continue;//继续装入下一页
	}
	else
	    // 如果引用页还未放栈顶,则为缺页,缺页数加1
	    FaultNumber++;
	//栈底页号记入淘汰页数组中
	EliminatePage[l] = PageFrames[FrameNumber-1];
	//向下压栈
	for(j=FrameNumber-1; j>0; j--)
	    PageFrames[j] = PageFrames[j-1];
	PageFrames[0] = next;//引用页放栈顶
	//报告当前实存中页号
	for(j=0; j<FrameNumber; j++)
	    if(PageFrames[j]>=0)
		cout << PageFrames[j] << " ";
	//报告当前淘汰的页号
	if(EliminatePage[l]>=0)
	    cout << "->" << EliminatePage[l++] << endl;
	else
	    cout << endl;
    }
    //分析统计选择的算法对于当前引用的页面走向的性能
    Report();
}
//先进先出置换算法
void Replace::Fifo(void)
{
    int i,j,k,l,next;
    InitSpace("FIFO");
    //循环装入引用页
    for(k=0,j=l=0; k<PageNumber; k++) {
	next = ReferencePage[k];
	//如果引用页已在实存中,报告实存页号
	for(i=0; i<FrameNumber; i++)
	    if(next == PageFrames[i])
		break;
	if (i < FrameNumber) {
	    for (i=0; i<FrameNumber; i++)
		if(PageFrames[i] >= 0)
		    cout << PageFrames[i] << " ";
	    cout << endl;
	    continue;// 继续引用下一页
	}
	//引用页不在实存中,缺页数加1
	FaultNumber++;
	EliminatePage[l] = PageFrames[j];//最先入页号记入淘汰页数组
	PageFrames[j] = next;//引用页号放最先入页号处
	j = (j+1)%FrameNumber;//最先入页号循环下移
	//报告当前实存页号和淘汰页号
	for(i=0; i<FrameNumber; i++)
	    if(PageFrames[i] >= 0)
		cout << PageFrames[i] << " ";

	if(EliminatePage[l] >= 0)
	    cout << "->" << EliminatePage[l++] << endl;
	else
	    cout << endl;
    }
    //分析统计选择的算法对于当前引用的页面走向的性能
    Report();
}

void Replace::Clock(void) 
{
    int i,j,k,l,next;
    InitSpace("Clock");
    //访问位数组
    bool *refbit = new bool[sizeof(bool)*FrameNumber];
    for(i=0; i<FrameNumber; i++) //将所有访问位初始化为0
	refbit[i] = false;
    //循环装入引用页
    for(k=0,j=l=0; k<PageNumber; k++) {
	next = ReferencePage[k];
	//如果引用页已在实存中,报告实存页号
	for(i=0; i<FrameNumber; i++)
	    if(next == PageFrames[i]) {
		refbit[i] = true; //当引用页被访问时，访问位置为1
		break;
	    }
	if (i < FrameNumber) {
	    for (i=0; i<FrameNumber; i++)
		if(PageFrames[i] >= 0)
		    cout << PageFrames[i] << " ";
	    cout << endl;
	    continue;// 继续引用下一页
	}
	//引用页不在实存中,缺页数加1
	FaultNumber++;
	i = j; //保存当前位置，检验是否将队列遍历完毕
	do {
	    if(refbit[j]) //如果当前页被访问过，给予二次机会
		refbit[j] = false; //把访问位置为0
	    else { //淘汰访问位为0的页
		EliminatePage[l] = PageFrames[j];//访问位为0的页号记入淘汰页数组
		PageFrames[j] = next;//引用页号放入当前页号处
		refbit[j] = true; //在页面刚被加入时，访问位置为1
		break;//放入了引用页后跳出
	    }
	    j = (j+1)%FrameNumber;//再从下一帧开始找未访问页
	} while(i!=j);
	if(i==j&&!refbit[j]) { //队列被完全走过1遍的情况
	    EliminatePage[l] = PageFrames[j];
	    PageFrames[j] = next;
	    refbit[j] = true; 
	}
	//报告当前实存页号和淘汰页号
	for(i=0; i<FrameNumber; i++)
	    if(PageFrames[i] >= 0)
		cout << PageFrames[i] << " ";

	if(EliminatePage[l] >= 0)
	    cout << "->" << EliminatePage[l++] << endl;
	else
	    cout << endl;
    }
    //分析统计选择的算法对于当前引用的页面走向的性能
    Report();
}

void Replace::Eclock(void)
{
    int i,j,k,l,next;
    InitSpace("Eclock");
    //访问位数组
    bool *refbit = new bool[sizeof(bool)*FrameNumber];
    bool *modbit = new bool[sizeof(bool)*FrameNumber];
    for(i=0; i<FrameNumber; i++) {//将所有访问位和修改位初始化为0
	refbit[i] = false;
	modbit[i] = false;
    }
    //循环装入引用页
    for(k=0,j=l=0; k<PageNumber; k++) {
	next = ReferencePage[k];
	//如果引用页已在实存中,报告实存页号
	for(i=0; i<FrameNumber; i++)
	    if(next == PageFrames[i]) {
		refbit[i] = true; //当引用页被访问时，访问位置为1
		modbit[i] = rand()%2; //用随机数模拟这一页是否会被修改
		break;
	    }
	if (i < FrameNumber) {
	    for (i=0; i<FrameNumber; i++)
		if(PageFrames[i] >= 0)
		    cout << PageFrames[i] << " ";
	    cout << endl;
	}
	else{
	    //引用页不在实存中,缺页数加1
	    FaultNumber++;
	    i = j; //保存当前位置，检验是否将队列遍历完毕
	    int replacePageNum = -1; //保存会被置换的页号
	    do {
		if(refbit[j]) {//如果当前页被访问过，给予二次机会
		    if(!modbit[j]) 
			replacePageNum = j; //如果未被修改，可能被替换
		    refbit[j] = false; //把访问位置为0
		}
		else if(modbit[j])
		    replacePageNum = j; //未被访问但被修改了，可能被替换
		else { //未被访问也未被修改，即找到了最佳替换页
		    replacePageNum = j; 
		    break;//立刻跳出
		}
		j = (j+1)%FrameNumber;//再从下一帧开始找未访问页
	    } while(i!=j);
	    if(-1 != replacePageNum) { //替换选中的被置换页
		EliminatePage[l] = PageFrames[replacePageNum];
		PageFrames[replacePageNum] = next;
		refbit[replacePageNum] = true; 
		modbit[replacePageNum] = rand()%2; //用随机数模拟这一页是否会被修改
	    } else { //队列完全被遍历1遍，且一直没有找到可选置换页面
		EliminatePage[l] = PageFrames[j];
		PageFrames[j] = next;
		refbit[j] = true; 
		modbit[j] = rand()%2; //用随机数模拟这一页是否会被修改
	    }
	    //报告当前实存页号和淘汰页号
	    for(i=0; i<FrameNumber; i++)
		if(PageFrames[i] >= 0)
		    cout << PageFrames[i] << " ";

	    if(EliminatePage[l] >= 0)
		cout << "->" << EliminatePage[l++] << endl;
	    else
		cout << endl;
	}
	for(i=0; i<FrameNumber; i++)
	    cout << "(" << refbit[i] << ',' << modbit[i] << ") ";
	cout << endl;
    }
    //分析统计选择的算法对于当前引用的页面走向的性能
    Report();
}

void Replace::Lfu(void)
{
    int i,j,k,l,next;
    InitSpace("Lfu");
    int *frequencyRecord = new int[FrameNumber]; //每一帧的被访问次数统计记录，按从大到小的顺序排列
    for(k=0,j=l=0; k<PageNumber; k++) {
	next = ReferencePage[k];
	for(i=0; i<FrameNumber; i++)
	    if(PageFrames[i]==next) {
		frequencyRecord[i]++; //被访问了，次数加一
		int tmpPF = PageFrames[i];
		int tmpFR = frequencyRecord[i];
		for(; i>0&&tmpFR>frequencyRecord[i-1]; i--) { //当前面一项的已访问次数小于当前项时，替换前一项
		    PageFrames[i] = PageFrames[i-1];
		    frequencyRecord[i] = frequencyRecord[i-1];
		}
		PageFrames[i] = tmpPF;
		frequencyRecord[i] = tmpFR;
		break;
	    }
	if(i==FrameNumber) { //当未找到引用页时
	    FaultNumber++; //缺页数加一
	    EliminatePage[l] = PageFrames[j]; //淘汰页为访问次数最小的一页
	    PageFrames[j] = next; //将新页面放入，访问次数为1
	    frequencyRecord[j] = 1;
	    if(j<FrameNumber-1)
		j++;
	}
	//报告当前实存页号、对应的访问次数和淘汰页号
	for(i=0; i<FrameNumber; i++)
	    if(PageFrames[i] >= 0)
		cout << PageFrames[i] << " ";

	if(EliminatePage[l] >= 0)
	    cout << "->" << EliminatePage[l++] << endl;
	else
	    cout << endl;

	for(i=0; i<FrameNumber; i++)
	    if(PageFrames[i] >= 0)
		cout << frequencyRecord[i] << " ";
	cout << endl;
    }
    //分析统计选择的算法对于当前引用的页面走向的性能
    Report();
}

void Replace::Mfu(void)
{
    int i,j,k,l,next;
    InitSpace("Mfu");
    int *frequencyRecord = new int[FrameNumber]; //每一帧的被访问次数统计记录，按从大到小的顺序排列
    for(k=0,j=FrameNumber-1,l=0; k<PageNumber; k++) {
	next = ReferencePage[k];
	for(i=0; i<FrameNumber; i++)
	    if(PageFrames[i]==next) {
		frequencyRecord[i]++; //被访问了，次数加一
		int tmpPF = PageFrames[i];
		int tmpFR = frequencyRecord[i];
		for(; i>0&&tmpFR>frequencyRecord[i-1]; i--) { //当前面一项的已访问次数小于当前项时，替换前一项
		    PageFrames[i] = PageFrames[i-1];
		    frequencyRecord[i] = frequencyRecord[i-1];
		}
		PageFrames[i] = tmpPF;
		frequencyRecord[i] = tmpFR;
		break;
	    }
	if(i==FrameNumber) { //当未找到引用页时
	    FaultNumber++; //缺页数加一
	    EliminatePage[l] = PageFrames[j]; //淘汰页为访问次数最大的一页
	    //将新页面放入，访问次数为1，并放在空位或实存最后一页
	    if(j>0) {
		PageFrames[j] = next; 
		frequencyRecord[j] = 1;
		j--;
	    } else {
		for(i=0; i<FrameNumber-1; i++) {
		    PageFrames[i] = PageFrames[i+1];
		    frequencyRecord[i] = frequencyRecord[i+1];
		}
		PageFrames[i] = next; 
		frequencyRecord[i] = 1;
	    }
	}
	//报告当前实存页号、对应的访问次数和淘汰页号
	for(i=0; i<FrameNumber; i++)
	    if(PageFrames[i] >= 0)
		cout << PageFrames[i] << " ";

	if(EliminatePage[l] >= 0)
	    cout << "->" << EliminatePage[l++] << endl;
	else
	    cout << endl;

	for(i=0; i<FrameNumber; i++)
	    if(PageFrames[i] >= 0)
		cout << frequencyRecord[i] << " ";
	cout << endl;
    }
    //分析统计选择的算法对于当前引用的页面走向的性能
    Report();
}

int main(int argc, char *argv[]) {
    srand(time(0));
    Replace *vmrp = new Replace();
    vmrp->Lru();
    vmrp->Fifo();
    vmrp->Clock();
    vmrp->Eclock();
    vmrp->Lfu();
    vmrp->Mfu();
    delete vmrp;
    return 0;
}
