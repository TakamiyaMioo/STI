**目录**

- [1. 2023全国大学生电子设计竞赛H题原题](https://www.cnblogs.com/helesheng/p/17888312.html#_label0)
- [2. 审题和选择技术路径](https://www.cnblogs.com/helesheng/p/17888312.html#_label1)
- [3. 模拟加法电路](https://www.cnblogs.com/helesheng/p/17888312.html#_label2)
- [4. 嵌入式程序的总体框架](https://www.cnblogs.com/helesheng/p/17888312.html#_label3)
- [5. 相位误差算法以及相位校正的嵌入式实现](https://www.cnblogs.com/helesheng/p/17888312.html#_label4)
- [6. 输入信号C中混合的A、B的频率和种类的判断](https://www.cnblogs.com/helesheng/p/17888312.html#_label5)
- [7. 正、余弦常数表的产生和计算 ](https://www.cnblogs.com/helesheng/p/17888312.html#_label6)
- [8. 基于DMA的快速A/D和D/A转换](https://www.cnblogs.com/helesheng/p/17888312.html#_label7)
- [9. 重建信号之间相位差的控制（发挥部分第（2）问） ](https://www.cnblogs.com/helesheng/p/17888312.html#_label8)
- [10. 嵌入式程序开发中遇到的几个小问题及其解决办法 ](https://www.cnblogs.com/helesheng/p/17888312.html#_label9)
- [11. 测试结果](https://www.cnblogs.com/helesheng/p/17888312.html#_label10)
- [参考文献：](https://www.cnblogs.com/helesheng/p/17888312.html#_label11)

 

------

2023年又是全国大学生电子设计竞赛年，一如既往的指导学生死磕H题。8月2日看到公布的赛题，我自己还沾沾自喜，觉得今年学生用嵌入式系统和数字信号处理知识就可以完成这题，赛前都辅导过，应该成绩不差。哪想到结果大跌眼镜，不但成绩还不如往年，学生的解题思路更是各式各样……

直到这几天寒假才有空，我按照自己的思路， 在STM32上用数字信号处理的方法将这道题全部做了一遍，效果不错。现分享出来以飨读者和粉丝，博大家一笑尔！也供后续的学生和参赛者参考。（注：以下讲解未按照电赛组委会要求的报告标准格式撰写，而是使用了随笔形式，重点讲解解题思路。未来阅读此文的参赛者不可以此作为《设计报告》范本）

作为电子设计竞赛的省级评委以及一个从业多年教育工作者，在竞赛评奖中，我一方面看到了很多兄弟学校参赛队对H题的不同解法，获益良多。另一方面，我也发现了很多高校在电子电工、信号处理前、嵌入式系统等领域的教学短板。因此我会在接下来的解题过程中，在每个关键步骤之后总结一下相关教学中存在的问题，希望对电子电工教学水平的提高有所帮助。

先“叠个甲”：本文观点难免有失偏颇，有不妥之处还请各位教育专家大佬不要介怀，并批评指正。以下原创内容欢迎网友转载，但请注明出处：https://www.cnblogs.com/helesheng 

 

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 1. 2023全国大学生电子设计竞赛H题原题

 

下面是原题：

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208154937332-1506362942.png)

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208155047589-659216396.png)

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208155124344-1275532487.png)

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 2. 审题和选择技术路径

 本题的核心功能是题图1中的分离电路实现的信号 “分离”，光从名称理解，很容易让人想到对C信号进行滤波——把两个信号分开。但仔细思考后会发现，这样做的问题是滤波器的过渡带极窄，极端情况下95KHz和100KHz之间只相差5%。很难将两个信号分离干净：A'中含有B的频率；B'中含有A的频率。从而造成A'和B'两个信号的失真。提高信号分离度的有效方法只有提高滤波器的阶数：对模拟滤波器而言，由于常见电容的精度很难做到5%以上，高阶模拟滤波器很难实现准确的截止频率和很窄的过渡带；对数字滤波器而言，高阶的IIR滤波器很难稳定，高阶的FIR滤波器会使计算量大增，无法满足计算的实时性。（注：此处必须具备一定的信号处理实践知识，既要能够判断5%的过渡带很难实现，又要明白IIR的缺点在于稳定性，FIR滤波器的短板在于计算量是IIR的许多倍）

另一种实现思路是“重建”A'和B'两个信号，即让图1中的分离电路**重新生成**C信号中含有的A、B两个信号。有很多参赛者怀疑重建的信号是否满足题目要求。对此组委会官方在赛题公布后公开答疑，明确了重建信号是满足要求的。

“重建”信号方案的难度在于：由于信号发生器产生的A、B和重新产生的A'、B'信号频率都不可能是绝对准确的，其精度都在0.01%左右，也就是万分之一。对于100KHz的信号，最糟糕的情况下，两者之间每秒将相差±10个左右的波形。如果用示波器同时显示两个信号会出现如下现象：用A信号作为触发源，则A'信号每秒在屏幕上将向左或向右“滚动”最多20个波形，也就是无法做到原题第三部分“说明”第（5）条提到的用示波器能够“稳定不漂移”的显示原信号和重建信号。

解决问题的办法是：**不断地实时测量A与A'、B与B'之间的相位差，并根据这个相位差不断调整重建的A'和B'的相位**。由此可以产生多种不同的技术路径及相关的解决方案：

1. 全数字方案：对C信号采样，在数字域计算A、B的频率，并用DAC重建同频率的两路信号A'、B'。随后不停地重复计算输入的A、B与A'、B'的相位差，并调整A'、B'的相位。
2. 将测量相位差的工作交给模拟相位测量芯片（如AD8302）来完成，MCU通过其输出来获取实时相位差。
3. 把信号重建工作交给DDS(Direct Digital Frequency Synthesis，如AD9833/AD9854等)芯片来完成，MCU通过接口实时调整DDS产生信号的相位。
4. 把信号重建工作交给压控振荡器（VCO，voltage-controlled oscillator)来实现，VCO控制电压由相位差测量电路来产生。（注：由于VCO很难产生低频正弦波，本方案不太容易获得较好结果。）

非常高兴地看到，本省的参赛队分别尝试了上述除第4种以外的所有技术路径，有一个参赛队结合第2和第3种技术路径获得了相当不错的结果，获得了省级一等奖，而其他参赛队表现都不如人意。接下来我将用全数字方案来解决这个问题。具体使用的器件是我校教学中使用的STM32F1系列芯片，熟悉其他器件平台的读者可以参考我的算法，在你熟悉的平台上实现。算法对于MCU的基本要求是：至少具有一路1MSPS以上的A/D转换，两路1MSPS以上的D/A转换能力，以及片上乘加运算（MAC）指令，处理能力达到50MIPS以上。这些软硬件能力，目前主流的国产或知名大厂的大部分32位和部分16位嵌入式MCU或DSP平台都具备。

**本环节揭示的信号处理教学中的问题：** 

a) 在测试中发现很多参赛队采用了滤波器，甚至模拟滤波器作为解决这道题的基本技术路径。这揭示出我们的《信号与系统》、《数字信号处理》和《模拟电路基础》等课程的教学停留在理论教学层面——学生对于滤波器的过渡带有认知，但没有确切的感性认识。只是因为上课时讲过这个内容，参赛同学就认为滤波器能够解决实际工程中的一切问题，**缺乏解决“复杂工程问题”的能力。**

b) 实验教学中，学生接触和使用示波器的机会不足。导致其不理解示波器的触发功能。对波形“滚动”的成因及对策没有认知。

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 3. 模拟加法电路

题图1中的加法器使用运算放大器实现，电路如下图所示。电路本身没有什么技术含量，就是教科书上的加法器电路。但由于MCU上集成的A/D只能接受正电压，这里用了隔直电容C1和C2去除输入信号的直流偏置，并用电阻R4和R5分压产生的Vcc/2作为加法器输出信号的直流偏置Vref。这样不管输入信号A和B的直流偏置是0还是其他电压，只要幅度合适，都可以被MCU片上的A/D转换器采集。

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208160610236-1640223251.png)

图3 带直流偏置的模拟加法器

图3电路图左下角的是MCU处理后输出的DAC_CH1和DAC_CH2两个信号的LC低通滤波器，其作用是平滑D/A转换输出的阶梯。而整个模块使用了左上角的PMOD接口来和MCU板（图3未画出）相连。实物连接图如下所示。

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208160854942-1583144674.jpg)

 图4 模拟加法器实物连接图

**本环节揭示的电子电工教学中的问题：**

a) 模拟电路教学过于重视分立元件教学，对于运算放大器这一最重要的教学对象关注度不足。其实现代模拟系统中，除电力电子专业之外，直接使用分离的二、三极管的场合已经非常有限，没有必要因循守旧，抱持几十年不变的教学内容。

b) 学生对于“模拟电路工作点”，这一重要概念理解不深刻，不会用。原因是模拟电路教材中多数使用双电源电路，工作点天生为GND，不需要调整。但对于现代低功耗、手持式电路而言，多数使用单电源供电，调整模拟电路的工作点就成为必须掌握的技能。对此，模拟电路教材必须有所改变。

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 4. 嵌入式程序的总体框架

本题的嵌入式程序要完成两件事：其一判断输入C波形中的含有的信号A、B的频率和类型（正弦或三角波）；其二根据实时输入的C信号，不断的调整重建信号的相位，以保持二者的相位差基本不变。根据题目中“说明”部分（3）给出的要求可知，判断输入信号中两个频率成分的频率和类型的工作应该在按下“启动键”后完成，且只需要执行一次，对应于下图左侧部分。而实时调整重建信号的相位的工作则应该在判断完成后不断地重复执行，以保证后续随时用示波器观测A和A’、B和B’信号时都可以做到“稳定显示不漂移”。其程序流程对应下图中右侧的循环部分。

![img](https://img2024.cnblogs.com/blog/1380455/202401/1380455-20240118121011507-730794656.jpg)

图5 嵌入式程序整体流程图

***\*（\**注：上面的流程图中，灰色的两步分别对测试管脚TST_PIN进行了翻转的操作。这样做的目的是为了便于用示波器通过观测TST_PIN管脚电平变化，了解程序运行的状态。TST_PIN管脚的使用情况，详见本文11.2小节所述）** 

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 5. 相位误差算法以及相位校正的嵌入式实现

#### 1）相位误差算法理论分析

正如本文第2小节“审题”中分析的，本题最大的难点在于如**何实时地计算算法重新生成单频的A'、B'信号和C中的A、B信号之间的相位差**，并在随后的计算不断实时校正，将相位差保持在一个固定的值，从而实现在示波器上“稳定显示不漂移”。我通过对输入信号C进行高速A/D采样数字化后，再用信号处理算法计算C中的A、B和重建的A'和B'之间的相位差。

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208161805300-1782840673.png)

图6 理论上的C信号和合成它的A、B两个信号 

说到测量某个固定频率信号的相位，大部分人会首先想到FFT这个工具，原因是FFT算法是“数字信号处理”课程重点介绍的内容，且大部分MCU有FFT算法库可以直接调用。我最初也想从FFT的相位谱还原A、B信号初始相位：

在时域内截取指定长度（L）的C信号，并用FFT计算该信号的相位谱，然后在相位谱中读取**A、B信号频率位置对应对应的相位值。**

但不幸的是，我在MATLAB中仿真这一算法时，发现结果根本不对！分析频域信息时发现了下图所示的现象。

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208162523532-1017057749.png)

图7 FFT后的功率谱（兰）和相位谱（红）

仔细想来，这种现象也不奇怪：当采样率为1MSPS，信号长度为1024时，频率分辨率（也就是频域中两个点的间隔）为fs/L = 1M/1024 = 976.56Hz。理论上说，100KHz的信号所对应的频谱位置将在第102.4个点上，而离散傅里叶变换只能得到整数点上的频域信息，那么100KHz的时域信号能量将泄漏到第102和103个频谱位置上。这个问题对于功率谱并不重要，因为功率是标量可以直接对周边几个点上的功率谱直接叠加即可；但对于相位谱而言再想从周边几个点的相位谱恢复出非整数点对应频率（如100KHz）的相位就不容易了……

其实，多年前我还是学生的时候，曾经仔细思考过这个问题，后来看了胡广书老师的经典教科书《数字信号处理——理论算法与实现》**[1]**第三章第八小节《对正弦信号截断的原则》才对此释然的。胡老师的教科书是这样写的“**抽样频率应为信号频率的整数倍，抽样点数应包含整周期**”。这次做题再次翻开胡老师的书，又涌起“身不能至，心向往之”的崇拜之情。

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208162705676-276217949.jpg)

图8 《数字信号处理——理论算法与实现》第三章第八小节《对正弦信号截断的原则》

至此若仍然坚持FFT方案就需要把采样长度**L时间内输入信号的周期数调整为K个**。由于采样率fs，截取长度L，截取时长内周期数K和A、B信号频率f之间必须满足下式：

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208233834440-391470809.png)

 要保持K为整数就要保证（1）式的右边也是整数。但不幸的是为执行FFT快速算法，L一般应该是2的整数次幂；f是题目中给出的10KHz、15KHz、20KHz……95KHz和100KHz等固定值。也就要求采样率fs是一个非常精确的小数。如：L为1024，f为100KHz，K为100时fs为1.024MSPS；L为1024，f为95KHz，K为100时fs为0.9728MSPS。这种连续非整数的采样率fs，对于MCU集成的逐次逼近SAR式 A/D转换器几乎是无法做到的。（注：以常见的STM32F1系列MCU为例，在56MHz主频下，其片上集成的SAR A/D转换器使用DMA+硬件连续触发模式，可获得最高采样率1MSPS。改变MCU主频、A/D工作频率或采样触发模式可获得不同的采样率，但在100KSPS以上的高采样率下，无法再获准确的获得连续变化的采样率。具体采样过程代码实现和分析，请参见本文第8小节的介绍） 

到此我陷入了解这道题最大的困境，我的脱困思路是：**既然保证K为整数的难点在于L必须是2的整数次幂，而L为2的整数次幂是由于快速算法FFT带来的。那不如干脆放弃快速傅里叶算法FFT，回到离散傅里叶变换DFT的本源，直接计算指定频点上的频域值（含幅度谱值和相位谱值）**。另一方面，解本题也并不需要知道奈奎斯特频率以下所有谱线的信息，我只要计算C信号在10KHz、15KHz、20KHz……95KHz和100KHz等固定谱线上的幅度和相位即可！这也大大缓解了不使用FFT快速算法带来的计算量增加问题。下面是我所采用的具体算法，如有不妥之处欢迎读者和网友指正： 

(1)固定以系统可以达到的最高采样率1MSPS进行A/D和D/A转换，同时将算法使用的数据长度L固定为1000个采样点（即时长为1ms的信号）。

(2)在内存中生成采样率为1MSPS，长度L=1000个采样点，频率为10KHz、15KHz、20KHz……95KHz和100KHz的（共19种）正信号:

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208234237814-1650477513.png) 

以及19种余弦信号： 

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208234307908-1562612703.png)

(3)当需要校正相位误差时（假设此时可称为t0），就同t0时刻开始对C信号实施长度为L=1000个采样点，历时1ms的采样。采样结果记为： 

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208234420038-783496024.png) 

(4)用采样结果分别与19种不同频率的正/余弦信号实施相关计算： 

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208234546898-1553361350.png)

(5)根据离散傅里叶变换（DFT）理论，由于{Sj,i}和{Cj,i}是正交的，上述两个计算结果 与 之比等于实际信号 中频率为 的信号的初始相位 的正切值。即可以通过下式计算初始相位：

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208234654216-1219349193.png)

(6)当然，根据题意，混合信号C中只有可能混有两种频率的信号。找到这两种频率的方法很简单，就是在计算（4）式之前，先通过功率谱计算公式： 

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208234740090-1912547106.png)

得到所有可能得19种信号的功率谱，再找到Pj中最大的两个所对应的频率即可。 

(7) 将Pj最大的两个频率的![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231208234915181-1534697805.png)计算出来就是C中所含的A、B两个信号的相位差了。

当我把这个算法写完，发现这不就是软件无线电（Software Defined Radio(SDR）中的数字正交解调吗！仔细想想，好像还真是，这算法就是数字相位解调（Phase demodulation）——只是载波频率较低，以方便参赛者通过MCU就可以验证软件无线电算法。这道题的命题老师应该是谙熟通信的专家，此题既帮助学生深刻的理解了DFT的定义，又引出了软件无线电的算法核心思想，可谓用心良苦。

#### 2）相位误差校正的实现

##### (1)相位同步A/D采集

实现相位误差校正首先要计算输入信号A与重建信号A’，以及B与B’之间的相位差（以下描述以A与A’为例介绍）。如前所述，计算A与A’相位差的方法是将长度为L个点（含有整数个信号周期）的A与A’信号代入公式（4）。由于A’是算法自己生成的信号，可以直接从生成信号的表格中读取，不需要进行A/D采集。为简便起见，从表格中读取参与（4）式计算的A’信号是从0相位开始的，因此对A信号的采样也需要在D/A重建的A’信号刚好播放到0相位时开始。所以图5所示的程序流程图右侧的循环相位校正部分的开始，就是循环等待到D/A输出的A’信号相位达到0相位的地方。举例来讲，我开通了DMA通道传输完成中断（DMA控制D/A转换的代码实现方式请参见本文第8小节的介绍），并在中断服务程序中对全局变量do_ad_flag1置位。中断服务程序代码如下所示：

```
1 void DMA2_Channel3_IRQHandler(void){    
2     if(DMA_GetITStatus(DMA2_IT_TC3)) {//判断通道2是否传输完成
3        DMA_ClearITPendingBit(DMA2_IT_TC3);    //清除通道2传输完成标志位
4         do_ad_flag1 = 0x55;//把全局标志置位，表示可以同步开始AD转换了
5      }
6 }
```

这样当主程序代码对do_ad_flag1清零后，只需要等待到下一次do_ad_flag1被置位的时刻，就是A’信号播放一轮L个点结束，也就是0相位的时刻。随后就可以启动DMA控制的A/D转换，实现长度为L的信号采样（DMA控制A/D转换的代码实现方式请参见本文第8小节的介绍）。

```
1 do_ad_flag1 = 0;//标志启动AD标志清零，等待完成一次DAC的DMA传输中断中置位
2 while(do_ad_flag1 == 0);//一直等待到一轮DA转换的DMA完成，才同步开启AD转换的DMA
3 ADC_DMA_Config(); //启动ADC的DMA
```

##### (2)离散余弦变换中相关（correlation）算法的实现

根据图5所示的程序整体流程图，公式（2）、（3）所示的离散余弦变换需要在程序执行过程中不断执行，以计算A和B信号的初始相位。因此执行公式（2）、（3）算法将占去CPU的最大计算带宽，需要尽量优化，以缩短执行两次相位校正的时间间隔，达到使输出的重建信号和输入信号尽量相位同步，避免示波器信号“显示漂移”的目的。好在A和B可选的频率只有19种，且可以在程序开始运行伊始就确定A和B的频率（对应图5中左侧的流程，其算法将在本文第6小节详细介绍）。对A’进行相位校正时只需要计算一次公式（2）、（3）即可，即（2）、（3）中的j值是固定的。我对公式（2）、（3）的实现代码如下所示：

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
1 double corr1000_200(short* data, short* mask) {
2     double res = 0;
3     short i;
4     for(i = 0; i < 1000 ; i++)
5         res = res + (data[i] - 2048) * (mask[i%200] - 2048);//注意模版要减去直流偏置
6     return res;
7 }
```

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

其中，采集得到的信号Ti存储在指针*data所指向的数组中，固定频率的正、余弦信号SCj和CCj存储在*mask所指向的数组中。由图3所示的电路原理图可知，二者为了满足MCU的A/D和D/A的要求，都是加入了Vcc/2的直流偏置电压，在数字域对应的数值为212÷2=2048，我在计算之前减去了这个直流偏置。另外，由于正、余弦信号表格中的数据SCj和CCj具有周期性，我只制作了长度为200个点的表格，其他的4/5长度的数据则通过周期沿拓的方式产生，具体代码如上所示。至于为什么制作长度为200个点的表格理由较复杂，推导和计算过程详见本文第7小节。 

调用上面的代码，实现公式（2）、（3）的代码如下所示： 

```
//计算离散傅里叶变换在单个频率点上的实部和虚部
temp_double_sin = corr1000_200(ADCConvertedValue, DAC_SIN + 200*input_frq_index[0]);
temp_double_cos = corr1000_200(ADCConvertedValue, DAC_COS + 200*input_frq_index[0]);
```

 其中，信号频率的编号存储在数组元素input_frq_index[0]中，编号的原则是10KHz信号对应的编号为0, 15KHz信号对应的编号为1, 20KHz信号对应的编号为2,……，100KHz信号对应的编号为18。则编号（frq_index）与真实频率（f）的换算关系为：

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231209000600554-746336894.png)

 而19种不同频率的正、余弦数据表格的数据都存放在指针DAC_SIN和DAC_COS所指向的大表格中，并依次排放（具体数据参见本文第7小节）。那么指向所需信号数据的首地址的指针就是DAC_SIN + 200*input_frq_index[0]了。

在上述算法中，都有意识的使用了double数据类型，目的一是为了防止计算中的溢出现象；二是为了简化后续三角函数计算中的的数据类型转换。 

##### (3)相位差计算 

使用公式（4）计算重建信号A’、B’和A、B之间的相位差，其代码如下： 

```
1 temp_double_angle = atan2(temp_double_cos, temp_double_sin);
```

 注：正切函数图像如下所示

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231209000924084-1422095141.png)

 

图9 正切函数图像

 计算正切值就是已知上图中的x值（也就是角度）来计算y值，每个x都能对应唯一的y值。但本题需要反过来使用正切函数，已知y值（也就是正切值），计算x的角度值时，就会出现多个角度都满足正切值为y的情况。只使用中间一根曲线的来搜寻对应的x值，显然并不完备。 

回到本题实际的物理意义，两个信号相位差可能为[-π，π]中的任意角度，只使用中间一根曲线的x轴的范围则只可能在 [-π/2，π/2]之间。 

问题发生在上图的正切函数图像的y轴正切值——**只有一个值**，即正切计算三角关系的“对边”和“临边”的比，这个值没法反映“对边”和“临边”究竟哪个为正数，哪个为负数。只能得出角度在[-π/2，π/2]之间的结论，也就是相角向量只能在第一和第四相限；而要使计算得到的相角在[-π，π]之间，就必须知道“对边”和“临边”的符号。

具体来说，就是计算相位差时调用的反正切公式只能是C语言标准库中有两个参数的double atan2(double y, double x)，而不能是只有一个参数的double atan(double x)。 

##### (4)实时相位校正 

上面得到的相位差是弧度制的，而嵌入式程序只能控制D/A重建的信号进行相对延迟，因此程序首先要将相位差折算为合理的延迟时间。具体步骤如下：

如前所述，上面得到的相位差的值域在[-π/2，π/2]之间，既有可能是相位延迟，也有可能是相位提前。当重建信号相对于输入信号的相位提前时，相位差为负数，只需要将该负相位的绝对值折算为延迟时间即可。但当重建信号的相位落后于输入信号时，相位差为正数，无法通过简单的直接延迟来提前重建信号的相位。一种可行的办法是利用重建信号的周期性，让其延迟2π减去相位差，即进行一个周期的求补操作。延迟相位折算的代码如下。 

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
1 double pos_angle;//转换为延迟0-2pi，全部为整数的角度
2 double temp_double;
3 short temp_short;
4 if(angle < 0)//atan2函数计算得到的相位差在-pi到pi，但当其小于0时，DAC输出的信号无法提前相位，只能将所有相位全部延迟一个周期，即进行一个周期的求补操作
5         pos_angle = -angle;
6 else
7         pos_angle = 2*3.1415926 - angle;
```

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 其中的变量pos_angle的单位是弧度，将其折算成延迟时间的公式如（7）所示。其中的f为信号频率，frq_index为信号频率编号，其换算关系使用了公式（6）。

![img](https://img2024.cnblogs.com/blog/1380455/202401/1380455-20240115004326131-2062003950.png)

 为了实现精确相位调整，我直接对控制DAC和DMA重建波形的TIM6/TIM7进行延时（关于基于DMA的DAC信号重建的代码，请参见本文第8小节）。为获得最高的时间分辨率，TIM6/TIM7的时钟被配置可用的最高频率——系统时钟56MHz。（7）式的计算结果delay_time单位为秒（S），而实际使用的值却是delay_time中含有多少个TIM6/TIM7时钟周期，因此（7）式的结果应再乘以56M。化简后得到代码如下：

```
1 temp_double = 5600*pos_angle / (3.1415926*((double)frq_index + 2));
2 temp_short = temp_double + 0.5;//转换为整数的延迟时间，避免舍入误差
3 return temp_short;
```

 注：此处提醒读者注意，为在STM32F1系列上获得1MSPS的A/D采样率，本文所述的系统使用了56MHz的系统时钟。如果直接使用本文代码,一定要首先将系统时钟修改为56MHz！如果你使用的是意法半导体官方的固件库和程序模板，则需要再system_stm32f10x.c中，将系统时钟配置代码修改如下： 

/*!< Uncomment the line corresponding to the desired System clock (SYSCLK) 

  frequency (after reset the HSI is used as SYSCLK source) */ 

//#define SYSCLK_FREQ_HSE  HSE_Value 

//#define SYSCLK_FREQ_20MHz 20000000 

//#define SYSCLK_FREQ_36MHz 36000000

//#define SYSCLK_FREQ_48MHz 48000000

\#define SYSCLK_FREQ_56MHz 56000000

//#define SYSCLK_FREQ_72MHz 72000000 

言归正传，上面返回的计算结果temp_short是整数型，这样做的目的是方便将其写入TIM6的定时寄存器。而在进行数据类型转换之前在temp_double基础上加了0.5是为了防止舍入误差，以实现浮点型向整数型转换过程中的四舍五入操作。

TIM6是DAC1重建A’的定时器，这里使用了一个对定时器定时时间延迟更精确的小技巧：直接向当前定时数值中增加需要延迟的时钟数其代码如下。（注：上一段代码中计算得到的延迟时间，此时被转存到了数组delay_times[]的各个元素中。）

```
1 temp_int = TIM6->CNT;
2 temp_int = temp_int + 65535 - delay_times[0];
3 TIM6->CNT = temp_int;
```

相对应的，如果要实现对B’信号的相位校正，就要向控制DAC2的采样间隔的TIM7中增加延迟时钟数了，这里不再给出源码。

#### 本环节揭示的信号处理和嵌入式系统教学中的问题： 

a) 《信号与系统》和《数字信号处理》教学中对于相位谱的讲解不如功率谱深入，这可能是由于相位谱的物理意义不清晰造成的，但至少在通信领域相位谱具有非常明确的物理意义和使用价值，值得《信号与系统》和《数字信号处理》的任课教师考虑花费学时强调一下。教学中，本例可以作为很好的一个相位谱应用的实例。

b) 可以发现，最近几届电赛题正在回归电子“设计”本身。作为电子电工教学的“指挥棒”，赛题变得更加贴近我国本科教学的实际，回归基本原理、基本技能；放弃对新型元件、新方法的一味偏执追求。以本赛题为例：用离散傅里叶变换实现相位调制的解调是属于《信号与系统》、《数字信号处理》和《通信原理》等课程中需要学生理解和掌握的基本理论，对这些理论的掌握和应用能力就是求解此题的必备技能。但为了防止参赛者使用现成软件无线电方案（多基于FPGA或专用ASIC芯片），造成不公平现象，本赛题使用了较低的信号频率。这种做法有效的避免了参赛队将精力放在“术”的层面；而是把精力集中在学习本科阶段必须掌握的基本技能，以及提高解决复杂工程问题的能力，这些“道”层面的东西上。
这也提示我们作为基层电子电工教学工作者，不应该把过多的精力放在讲解FPGA使用的操作流程或者某个具体芯片的配置流程这些花哨的“雕虫小技”，而应该不断抓牢基本原理、基本技能和解决复杂工程问题这些重点上。

c) 学生缺乏对算法复杂度的估计能力，这一方面源于学生的工程不足。但更重要的原因是“双师型”教师缺乏，没有相当的工程经验的教师很难对自己设计算法的计算量进行有效估计，更难交给学生对算法复杂度进行估算的习惯和方法。

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 6. 输入信号C中混合的A、B的频率和种类的判断

####  1）频率的判断

C中混杂的两个分量A、B的频率只有可能是10KHz、15KHz……100KHz等共19种，所以对大多数参赛者而言，识别它们都不是一件特别困难的事。有同学使用FFT，当然也可以使用相关计算DFT。我自己在做的时候犹豫了一下，后来还是胡广书老师书中的话（图8）影响了我——如果使用长度L为2的整数次幂（如1024）点的FFT，则L内的数据对于所有19种频率都不刚好含有整数个周期，那么势必造成转换结果的频谱能量泄露。也就是不能以功率谱中单个点的幅度来代表信号在某个频率上的总能量，而要使用频谱中至少2-3个相邻点的功率之和，才能代表输入信号在单个频率上的功率。这种算法对于只有正弦输入的“基本要求”部分没有多大影响。但如果考虑“发挥部分”可能的三角波输入，就不得不考虑三角波在奇次谐波上的谐波失真和基频信号的叠加可能对功率谱的影响。

因此我决定仍然采用L=1000的DFT来计算信号在19个指定频率频点上的功率。**此时由于时域长度为L=1000的信号中，含有的所有信号都是整数个周期，信号的所有频谱只会有一根谱线。**在计算时无需像使用FFT算法一样，取相邻几个频点的功率，得到的功率值将更准确；在输入信号有可能是三角波时也不容易和其三次谐波混淆。加上前面所述的相位差算法中已经有了实现（2）、（3）两式的代码，只要再增加实现（5）式的代码即可。

```
1 //计算IQ分量的模的长度
2 double modulus(double I, double Q){
3     double temp_double;
4     temp_double = sqrt(I*I + Q*Q);
5     return temp_double;
6 }
```

DFT计算两个输入信号频率的代码罗列于下面，供网友参考。

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
 1 void cal_2frqs(unsigned char* frq)//计算参与叠加的两个正弦信号的频率
 2 {
 3     double corr_sin_double;//用于存放C与所有19种正弦相关计算的结果
 4     double corr_cos_double;//用于存放C与所有19种余弦相关计算的结果
 5     double mag[19];//用于存放输入信号与所有19种信号相关计算的模
 6     double temp_double[19],sort_temp;
 7     unsigned char i,j,temp_char;
 8     for(i = 0; i < 19 ; i++)
 9     {
10         corr_sin_double = corr1000_200(ADCConvertedValue, DAC_SIN + 200*i);
11         corr_cos_double = corr1000_200(ADCConvertedValue, DAC_COS + 200*i);
12         mag[i] = modulus(corr_sin_double, corr_cos_double);
13         temp_double[i] = mag[i]; 
14     }
15     //冒泡法对大小进行排序
16     for (i = 0; i < 19 - 1; i++)
17     {
18         for (j = 0; j < 19 - i - 1; j++){
19                 if (temp_double[j] < temp_double[j+1]){
20                         sort_temp = temp_double[j];
21                         temp_double[j] = temp_double[j+1];
22                         temp_double[j+1] = sort_temp;
23                 }
24             }
25     }
26     //寻找最大值序号
27     for (i = 0; i < 19; i++){
28         if(mag[i] == temp_double[0])//找到最大数
29             frq[0] = i;
30         if(mag[i] == temp_double[1])//找到第二大数
31             frq[1] = i;
32     }
33     if(frq[0] > frq[1]) {//把频率低的放在第一个位置，如果颠倒就换一下
34         temp_char = frq[0];
35         frq[0] = frq[1];
36         frq[1] = temp_char;
37     }
38 }
```

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

其中，值得注意的是计算出所有的19个相关结果的模后，我用冒泡法对这些模进行了排序。而这个冒泡法排序的代码是百度“文心一言”写的，不得不说这提高了对数据结构不太熟悉的人的编程速度。然后逐一搜索找到了最大的两个的模所在的频率编号i，并把它们放到了数组frq[ ]中。啰嗦一句，至此放在frq[ ]中的编号和实际频率之间的对应关系为公式（6）。而上述函数真实通过向形式参数指针所指向的数组写入数值的方式向外传递计算结果的。

该函数的最后，还对frq[0]和frq[1]中存储的频率编号进行了调整，使较低频率的信号放在frq[0]中，这样做的目的使D/A转换器1输出的信号频率永远小于D/A转换器2输出的信号频率。

#### 2）信号种类（正弦或三角波）的判断

“发挥部分”要求参赛者能够判断A和B信号究竟是正弦波还是三角波。从理论上讲这也并不困难——**三角波在奇次谐波上存在谐波分量**。为简便起见，我选择对3次谐波的能量进行计算——**并将其与基频功率进行比较，当达到一定比例时就认为这些频点上存在谐波失真**，也就是输入是三角波。这就要求程序能够计算频率在30KHz，45KHz，……，250KHz，275KHz，300KHz频率位置的功率谱，如果继续使用前述的不带快速算法的DFT，就还要给出上面这些频点的正、余弦数据，也就需要在现有的正、余弦数据表的基础上，还要增加100KHz频率以上的信号数据表。

而使用FFT则可以直接得到奈奎斯特频率以下的所有谱线功率，因此我偷了个懒，直接使用了FFT。为使信号长度为2的整数次幂，以方便FFT计算，我将DMA控制A/D采样的长度增加到1024。由于1024点的数据不可能含有整数个波形，频谱的能量也不会集中在一根谱线上。下表所示的是FFT长度为1024个点时，我们感兴趣的三次谐波谱线在FFT结果数组中的位置。

 表1 三次谐波频率在FFT结果中的位置 

![img](https://img2024.cnblogs.com/blog/1380455/202401/1380455-20240120140446453-698177776.png) 

可以发现，感兴趣的频点并不一定在整数点上。为防止相邻谱线中的能量泄露导致的问题，我**对感兴趣频率谱线位置周边3个相邻点的功率进行了求和**。

不过上述算法存在一个重大漏洞：当被测信号C中混合的A、B两个单频信号中的一个的频率刚好是另一个信号频率的3倍时（例如A为10KHz，B为30KHz时），低频信号的3次谐波会和较高频率信号的能量混淆在一起，从而使算法是无法判断低频信号是三角波还是正弦波的。但幸运的是，根据题意这种情况只会出现在三次谐波频率低于最高频率100KHz的情况下。具体来说会在（1）低频信号是10KHz，高频信号是30KHz；（2）低频信号是15KHz，高频信号是45KHz；（3）低频信号是20KHz，高频信号是60KHz；（4）低频信号是25KHz，高频信号是75KHz；（5）低频信号是30KHz，高频信号是90KHz。共五种情况下出现这个问题。

我在上述五种情况下，对低频信号的五次谐波进一步进行判断，方法与其他频点上对三次谐波的判断相同。

下面是我使用意法半导体提供的STM32F1的DSP库进行波形种类识别的函数代码。 

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
 1 void     fft_cal_2types(unsigned char* frq, unsigned char* wave_type)//根据输入信号及已经判断出来的频率值，通过FFT结果判断信号类型是三角波（1）还是正弦波（0）
 2 //第一个参数指针指向的数组，其中存放的是之前的其他函数计算出来的两个信号的频率
 3 //第二个参数指正指向的数组用于存放这两个信号的波形种类，0为正弦，其他值为三角波。需要在本函数中计算产生
 4 {
 5     short indata[2048],outdata[2048],i;
 6     int mag[512],sum=0,average;
 7     unsigned short index;//输入信号的频率对应的索引值
 8     unsigned short harmo3_index,harmo5_index;//输入信号三次和五次谐波频率对应的索引值
 9     float temp_float;
10     float mag_1harmo,mag_3harmo,k;//一次和三次谐波上的能量及其比值
11     float mag_5harmo;//五次谐波上的能量
12     _Bool conti;
13 
14     for(i = 0; i < 1024 ; i++)//计算信号平均值
15         sum = sum + ADCConvertedValue[i];
16     average = sum/1024;
17     for(i = 0; i < 1024 ; i++)
18     {
19         indata[2*i] = ADCConvertedValue[i] - average;//减去平均值是为了降低直流分量
20         indata[2*i + 1] = 0;
21     }
22     cr4_fft_1024_stm32(outdata, indata, 1024);//注意阅读关于输入输出数据存放格式的描述    
23     for(i = 0; i < 512 ; i++)//FFT结果幅度谱是左右对称的，只需要算一半
24         mag[i] = outdata[2*i] * outdata[2*i] + outdata[2*i + 1] * outdata[2*i + 1];//sqrt(outdata[2*i] * outdata[2*i] + outdata[2*i + 1] * outdata[2*i + 1]);
25     
26     //////////计算输入信号频率及其三次谐波频率对应在FFT结果数组中的索引值//////////
27     //////初步判断第一个信号的类型//////////////
28     temp_float = (float)frq[0]*5 + 10;//计算输入索引值所对应的真实频率，单位为KHz
29     temp_float = temp_float / 500;    //采样率为1MSPS，则输入频率是奈奎斯特频率的多少倍
30     temp_float = temp_float * 512;    //计算输入信号频率对应的数组索引
31     index = temp_float + 0.5;//进行四舍五入取整
32     
33     temp_float = (float)frq[0]*5 + 10;//计算输入索引值所对应的真实频率，单位为KHz
34     temp_float = temp_float / 500;    //采样率为1MSPS，则输入频率是奈奎斯特频率的多少倍
35     temp_float = temp_float *3 * 512;    //计算三倍基频对应的数组索引
36     harmo3_index = temp_float + 0.5;//进行四舍五入取整    
37     
38     mag_1harmo = mag[index-1] +  mag[index] +  mag[index+1];
39     mag_3harmo = mag[harmo3_index-1] +  mag[harmo3_index] +  mag[harmo3_index+1];
40     k = mag_1harmo/mag_3harmo;
41     if(k > 300)
42         wave_type[0] = 0;//三次谐波较小，说明是正弦波
43     else
44         wave_type[0] = 1;//三次谐波较大，说明是三角波
45     //////////初步判断第二个信号的类型////////////////
46     temp_float = (float)frq[1]*5 + 10;//计算输入索引值所对应的真实频率，单位为KHz
47     temp_float = temp_float / 500;    //采样率为1MSPS，则输入频率是奈奎斯特频率的多少倍
48     temp_float = temp_float * 512;    //计算输入信号频率对应的数组索引
49     index = temp_float + 0.5;//进行四舍五入取整
50     
51     temp_float = (float)frq[1]*5 + 10;//计算输入索引值所对应的真实频率，单位为KHz
52     temp_float = temp_float / 500;    //采样率为1MSPS，则输入频率是奈奎斯特频率的多少倍
53     temp_float = temp_float *3 * 512;    //计算三倍基频对应的数组索引
54     harmo3_index = temp_float + 0.5;//进行四舍五入取整    
55 
56     mag_1harmo = mag[index-1] +  mag[index] +  mag[index+1];
57     mag_3harmo = mag[harmo3_index-1] +  mag[harmo3_index] +  mag[harmo3_index+1];
58     k = mag_1harmo/mag_3harmo;
59     if(k > 300)
60         wave_type[1] = 0;//三次谐波较小，说明是正弦波
61     else
62         wave_type[1] = 1;//三次谐波较大，说明是三角波
63     
64     ////////对第二个信号刚好等于第一个信号三倍频率特殊情况，需要重新判断第一个信号的类型/////
65     //上面的初步判断算有问题的是：第二个信号四第一个信号三倍频率的情况，只有可能包括：
66     //1、第一个信号是10K，第二个信号是30K
67     //2、第一个信号是15K，第二个信号是45K
68     //3、第一个信号是20K，第二个信号是60K
69     //4、第一个信号是25K，第二个信号是75K
70     //5、第一个信号是30K，第二个信号是90K
71     conti = ((frq[0] == 0)&(frq[1] == 4))|((frq[0] == 1)&(frq[1] == 7))|((frq[0] == 2)&(frq[1] == 10))|((frq[0] == 3)&(frq[1] == 13))|((frq[0] == 4)&(frq[1] == 16));
72     if(conti)
73     {
74             //计算五次谐波能量和1次谐波能量之比
75             temp_float = (float)frq[0]*5 + 10;//计算输入索引值所对应的真实频率，单位为KHz
76             temp_float = temp_float / 500;    //采样率为1MSPS，则输入频率是奈奎斯特频率的多少倍
77             temp_float = temp_float * 512;    //计算输入信号频率对应的数组索引
78             index = temp_float + 0.5;//进行四舍五入取整
79             mag_1harmo = mag[index-1] +  mag[index] +  mag[index+1];
80             temp_float = (float)frq[0]*5 + 10;//计算输入索引值所对应的真实频率，单位为KHz
81             temp_float = temp_float / 500;    //采样率为1MSPS，则输入频率是奈奎斯特频率的多少倍
82             temp_float = temp_float * 5 * 512;    //计算5倍基频对应的数组索引
83             harmo5_index = temp_float + 0.5;//进行四舍五入取整    
84             mag_5harmo = mag[harmo5_index-1] +  mag[harmo5_index] +  mag[harmo5_index+1];
85             k = mag_1harmo/mag_5harmo;
86             if(k > 500)
87                 wave_type[0] = 0;//5次谐波较小，说明是正弦波
88             else
89                 wave_type[0] = 1;//5次谐波较大，说明是三角波
90     }
91 }
```

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 这段代码中应注意到：

（1）由于STM32的A/D转换器只能接受正电压，注意上面的代码在进行FFT之前，首先对所有信号减去了它们的平均值（即直流偏置），目的是去除FFT频谱中的直流分量。

（2）官方库的FFT函数的输入是复数，而本题输入的时域信号只有实部，因此将输入数组indata[ ]的奇数地址全部置0。

（3）各个频率谐波在FFT结果中对应的位置计算中，我首先使用浮点型来保持其精度，然后根据C语言强制类型转换的方式是舍弃法的特点。我在强制类型转换为整形之前，对其进行了+0.5的操作。

（4）根据真实测试的结果总结，我取三次谐波mag_3harmo大于一次谐波mag_1harmo的1/300时就认为这个频点上的信号是三角波。

（4）布尔型变量conti的值conti = ((frq[0] == 0)&(frq[1] == 4))|((frq[0] == 1)&(frq[1] == 7))|((frq[0] == 2)&(frq[1] == 10))|((frq[0] == 3)&(frq[1] == 13))|((frq[0] == 4)&(frq[1] == 16));代表了较高频信号的频率是否刚好等于低频信号的三倍（即上述5中特殊情况），而对于五次谐波的判断标准则被降低到基波能量的1/500。

（5）fft_cal_type()函数使用的内存远远大于MDK启动文件中分配给“栈”的空间，这将造成内存溢出错误。关于堆和栈内存重新分配的问题，请参见本文第10小节的描述和讲解。

**本环节揭示出的信号处理、嵌入式系统教学中的问题：**

a) 大模型（ChatGPT、通义千问、文心一言）的编程功能，必将对嵌入式系统和信号处理类课程产生深远的影响。**人类历史一再证明，对于新工具采取对抗态度的人必将被时代淘汰！**与其禁止学生使用大模型工具编程，不如持开放态度。至少在目前的阶段，大模型还无法直接归纳、总结和创造如本题这样复杂工程问题的整体解决方案。何不将学生从“冒泡法”这样相对较死的问题中解放出来去思考更有意义的问题。但反过来讲，在教学中照本宣科，讲解“冒泡法”这种固定答案问题的课程，可能也到了不得不“死”的时代。

b) 教学中**对于常见的嵌入式系统功能固件库讲解和实验不足**。例如本题用到的DSP库或安全加密库Cryptographic library都是解决复杂问题时常用的固件库，有必要在教学中添加重要的官方固件库使用的教学内容。但这些库的使用涉及除嵌入式系统外的知识，适合放在相关专业必修和专业选修课中作为课程设计或专业实习的内容。
目前一种合理的课程体系设计方案是**将选修课分为不同方向的“课程群”，这样做就可以将相关固件库的教学内容放在课程群的综合实践环节来进行了。**例如，DSP库可以放在信号处理选修课程的综合实践环节；以太网应用库和Cryptographic库则可以放在网络及其安全课程群等等。 

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 7. 正、余弦常数表的产生和计算 

 如前所述的（2）、（3）两式所示的离散傅里叶变换的计算，需要知道知道正、余弦常数表格{Sj,i}和{Cj,i}。当然你可以根据上一节所示的计算公式调用正、余弦函数来实时地计算这些常数。但由于（2）、（3）两式的计算是本题程序中耗时最多、最频繁调用的计算，在（2）、（3）两式中实时计算{Sj,i}和{Cj,i}会使整个算法的复杂度提高2个以上的数量级！

#### 1)常见的解决办法

常见的解决办法有二：

其一，将所有的

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231209121701728-1410427314.png)

 **全部**用MATLAB或Python等工具事先算出，并把它们事先存放在MCU的RAM当中，使用时再用“**查表法**”逐一读出。

其二，只在MCU的RAM中存储**一张**长度为1000个点（或更长）的正弦数值表格，并在需要{Sj,i}和{Cj,i}数值时调用**DDS（Direct Digital Frequency Synthesis）算法**来生成不同频率（由常数j代表）的正、余弦信号数值。

方法一的优点是简单粗暴、实现容易，且节约了方法二中DDS算法生成查表地址的时间。同时，方法一的缺点是将占用数十倍于方法二的RAM存储空间。另外，我还觉得：在本题中，离散余弦变换只需要在指定频点上进行，能够生成连续可调频率的DDS算法有“杀鸡用牛刀”的嫌疑；且DDS算法在生成信号时使用邻近位置的数值代替本点数值的方法，会对生成正、余弦信号的信噪比有一定的损失。好在当代MCU的RAM空间往往达到数十到数百KB，基本具备了实现方法一的硬件条件。（注：也许有网友想把表格用const关键词修饰，放到MCU的Flash中。但这样做有可能导致D/A播放波形时DMA访问速度降低（STM32的Flash访问宽度时64bits，关于这个问题大家可以参阅我写的教科书：[《基于STM32的嵌入式系统原理及应用》](http://product.dangdang.com/29334121.html)（科学出版社出版 ISBN:9787030697974））。因此我并未使用这种方法，大家可以自行尝试一下。

#### 2)正、余弦常数表的生成

为在（2）、（3）两式中正、余弦常数{Sj,i}和{Cj,i}，正余弦常数表中存储的波形应该与上一节“相位误差算法理论分析”中提到的采样信号相同：采样率fs，截取长度L，截取时长内周期数K和A、B信号频率f之间必须满足（1）式。当fs为1MSPS、L为1000，f分别等于题目要求的10KHz、15KHz、20KHz……95KHz和100KHz时，可得到长度为L的表格中正弦周期数K分别为：10、15、20、25、30、35、40、45、50、55、60、65、70、75、80、85、90、95、100。当频率一定时，正、余弦周期内的波形数据完全相同；但不同频率下的正、余弦周期内的数值却不尽相同。即频率为10KHz时K=10个波形的数值完全相同；15KHz时K=15个波形的数值完全相同；……。而相同的波形数值只需要存储一次即可，不必真的存储10个、15个……95个周期的数据。否则，数据表格占用的RAM数量将达到2×19×1000=38K个数据点，以每个点2字节计，可达72KB，这已经超过了大多数MCU的RAM容量。而**计算时不足需的其他数据则通过周期延拓产生**。

那么问题来了，如果真的只为每种频率的信号存储长度为一个周期的信号，那么这些表格的长度不相等。从而造成对不同频率的信号计算（2）、（3）两式使用的程序不尽相同，很难实现通用于所有输入频率的代码。我耍个小聪明：**计算所有K值的最大公约数——为5，因此只需要为每种波形数据存储L÷5=200个点的数据即可满足要求！**因为在200个数据点中，所有可能频率的周期数都是整数。这样RAM中存储的10KHz的信号含有2个完整周期，15KHz的信号含有3个完整周期，20KHz的信号含有4个完整周期，……，95KHz的信号含有19个完整周期，100KHz的信号含有20个完整周期。

另外，为降低计算的复杂度，压缩计算时间，我使用**定点计算**来实现（2）、（3）两式。其中的A/D转换结果{Tj,i}已经是定点数，还需要将正、余弦常数表也转换为两字节无符号的unsigned short类型。我使用下面的MATLAB代码来实现。其中2048是12位无极性A/D转换器输出数值的中值。而1500是表中存储的正余弦数据的幅度。

```
N=200;``i``=1:N;``s_sin10k = ``round``(1500*``sin``(2*``pi``*``i``/N*2) + 2048);``%10K信号对应每200个点（采样率为1MSPS）内有两个正弦周期``s_sin15k = ``round``(1500*``sin``(2*``pi``*``i``/N*3) + 2048);``%15K信号对应每200个点（采样率为1MSPS）内有3个正弦周期``s_sin20k = ``round``(1500*``sin``(2*``pi``*``i``/N*4) + 2048);``%20K信号对应每200个点（采样率为1MSPS）内有4个正弦周期``s_sin25k = ``round``(1500*``sin``(2*``pi``*``i``/N*5) + 2048);``%25K信号对应每200个点（采样率为1MSPS）内有5个正弦周期``s_sin30k = ``round``(1500*``sin``(2*``pi``*``i``/N*6) + 2048);``%30K信号对应每200个点（采样率为1MSPS）内有6个正弦周期``s_sin35k = ``round``(1500*``sin``(2*``pi``*``i``/N*7) + 2048);``%35K信号对应每200个点（采样率为1MSPS）内有7个正弦周期``s_sin40k = ``round``(1500*``sin``(2*``pi``*``i``/N*8) + 2048);``%40K信号对应每200个点（采样率为1MSPS）内有8个正弦周期``s_sin45k = ``round``(1500*``sin``(2*``pi``*``i``/N*9) + 2048);``%45K信号对应每200个点（采样率为1MSPS）内有9个正弦周期``s_sin50k = ``round``(1500*``sin``(2*``pi``*``i``/N*10) + 2048);``%50K信号对应每200个点（采样率为1MSPS）内有10个正弦周期``s_sin55k = ``round``(1500*``sin``(2*``pi``*``i``/N*11) + 2048);``%55K信号对应每200个点（采样率为1MSPS）内有11个正弦周期``s_sin60k = ``round``(1500*``sin``(2*``pi``*``i``/N*12) + 2048);``%60K信号对应每200个点（采样率为1MSPS）内有12个正弦周期``s_sin65k = ``round``(1500*``sin``(2*``pi``*``i``/N*13) + 2048);``%65K信号对应每200个点（采样率为1MSPS）内有13个正弦周期``s_sin70k = ``round``(1500*``sin``(2*``pi``*``i``/N*14) + 2048);``%70K信号对应每200个点（采样率为1MSPS）内有15个正弦周期``s_sin75k = ``round``(1500*``sin``(2*``pi``*``i``/N*15) + 2048);``%75K信号对应每200个点（采样率为1MSPS）内有15个正弦周期``s_sin80k = ``round``(1500*``sin``(2*``pi``*``i``/N*16) + 2048);``%80K信号对应每200个点（采样率为1MSPS）内有16个正弦周期``s_sin85k = ``round``(1500*``sin``(2*``pi``*``i``/N*17) + 2048);``%85K信号对应每200个点（采样率为1MSPS）内有17个正弦周期``s_sin90k = ``round``(1500*``sin``(2*``pi``*``i``/N*18) + 2048);``%90K信号对应每200个点（采样率为1MSPS）内有18个正弦周期``s_sin95k = ``round``(1500*``sin``(2*``pi``*``i``/N*19) + 2048);``%95K信号对应每200个点（采样率为1MSPS）内有19个正弦周期``s_sin100k = ``round``(1500*``sin``(2*``pi``*``i``/N*20) + 2048);``%100K信号对应每200个点（采样率为1MSPS）内有20个正弦周期` `s_cos10k = ``round``(1500*``cos``(2*``pi``*``i``/N*2) + 2048);``%10K信号对应每200个点（采样率为1MSPS）内有两个正弦周期``s_cos15k = ``round``(1500*``cos``(2*``pi``*``i``/N*3) + 2048);``%15K信号对应每200个点（采样率为1MSPS）内有3个正弦周期``s_cos20k = ``round``(1500*``cos``(2*``pi``*``i``/N*4) + 2048);``%20K信号对应每200个点（采样率为1MSPS）内有4个正弦周期``s_cos25k = ``round``(1500*``cos``(2*``pi``*``i``/N*5) + 2048);``%25K信号对应每200个点（采样率为1MSPS）内有5个正弦周期``s_cos30k = ``round``(1500*``cos``(2*``pi``*``i``/N*6) + 2048);``%30K信号对应每200个点（采样率为1MSPS）内有6个正弦周期``s_cos35k = ``round``(1500*``cos``(2*``pi``*``i``/N*7) + 2048);``%35K信号对应每200个点（采样率为1MSPS）内有7个正弦周期``s_cos40k = ``round``(1500*``cos``(2*``pi``*``i``/N*8) + 2048);``%40K信号对应每200个点（采样率为1MSPS）内有8个正弦周期``s_cos45k = ``round``(1500*``cos``(2*``pi``*``i``/N*9) + 2048);``%45K信号对应每200个点（采样率为1MSPS）内有9个正弦周期``s_cos50k = ``round``(1500*``cos``(2*``pi``*``i``/N*10) + 2048);``%50K信号对应每200个点（采样率为1MSPS）内有10个正弦周期``s_cos55k = ``round``(1500*``cos``(2*``pi``*``i``/N*11) + 2048);``%55K信号对应每200个点（采样率为1MSPS）内有11个正弦周期``s_cos60k = ``round``(1500*``cos``(2*``pi``*``i``/N*12) + 2048);``%60K信号对应每200个点（采样率为1MSPS）内有12个正弦周期``s_cos65k = ``round``(1500*``cos``(2*``pi``*``i``/N*13) + 2048);``%65K信号对应每200个点（采样率为1MSPS）内有13个正弦周期``s_cos70k = ``round``(1500*``cos``(2*``pi``*``i``/N*14) + 2048);``%70K信号对应每200个点（采样率为1MSPS）内有15个正弦周期``s_cos75k = ``round``(1500*``cos``(2*``pi``*``i``/N*15) + 2048);``%75K信号对应每200个点（采样率为1MSPS）内有15个正弦周期``s_cos80k = ``round``(1500*``cos``(2*``pi``*``i``/N*16) + 2048);``%80K信号对应每200个点（采样率为1MSPS）内有16个正弦周期``s_cos85k = ``round``(1500*``cos``(2*``pi``*``i``/N*17) + 2048);``%85K信号对应每200个点（采样率为1MSPS）内有17个正弦周期``s_cos90k = ``round``(1500*``cos``(2*``pi``*``i``/N*18) + 2048);``%90K信号对应每200个点（采样率为1MSPS）内有18个正弦周期``s_cos95k = ``round``(1500*``cos``(2*``pi``*``i``/N*19) + 2048);``%95K信号对应每200个点（采样率为1MSPS）内有19个正弦周期``s_cos100k = ``round``(1500*``cos``(2*``pi``*``i``/N*20) + 2048);``%100K信号对应每200个点（采样率为1MSPS）内有20个正弦周期
```

　现将上述MATLAB脚本得到的正余弦表罗列于下，共后继参赛者培训时直接取用。　

![img](https://images.cnblogs.com/OutliningIndicators/ContractedBlock.gif) 正、余弦数据表

这里值得注意的技巧是，我将不同频率的所有正弦数据放在一个数组中，所有余弦数据放在另一个数组中，这样做有利于在程序执行时根据前面计算的结果访问指定频率的数据。例如，已知输入A信号的频率为30KHz，折算成频率的编号为j=4，则指向表格中频率为30KHz的正弦信号数据的首地址指针即为(DAC_SIN + 200*j)；30KHz的余弦信号数据的首地址指针即为(DAC_COS + 200*j)。

综上，所有表格占用的内存大小将为19*2*200*2=15.2KB。常见的STM32的RAM空间无疑都能够满足要求。 

**本环节揭示出的嵌入式系统教学中的问题：**

a) 有的学生不理解计算机实现算法时“**查表法**”的重要意义，痴迷于使用解析法得到所需的所有数据，大大限制了程序执行效率。

b) 在何种程度上使用查表法，也是值得算法开发者针对具体问题仔细斟酌的问题，因为庞大的表格占用的资源也是嵌入式系统不得不面对的问题。

上述两个问题过于具体，而对于嵌入式教学的指导意义，仍然指向需要学校进一步加大实践教学力度，提升嵌入式系统教师的水平，提升具有开发经验的双师型教师的比例。或许加大线上线下混合教学的力度，进一步出版真正有技术含量的教科书也是不错的解决方案。在这里在推荐一下本教学团队的教材：[《基于STM32的嵌入式系统原理及应用》](http://product.dangdang.com/29334121.html)（科学出版社出版 ISBN:9787030697974）和我个人的B站账号“[何乐生0](https://space.bilibili.com/1201496332)”，欢迎大家购买和关注。

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 8. 基于DMA的快速A/D和D/A转换

#### 1)高速A/D转换的实现

根据题意，被处理的信号最高频率为100KHz，而三角波与正弦波的频谱相比，主要差异在奇次谐波，因此要对100KHz的三角波进行分析，至少要保留300KHz处的谱线信息，其奈奎斯特采样率为600KSPS。我选择STM32F1系列作为数字信号处理平台，其使用一个A/D转换器的最高采样率为1MSPS，能够满足需要。但在如此高的采样率下，任何MCU都只有使用DMA模式控制A/D转换，才能够保证足够小的采样间隔时间孔径抖（aperture jitter），以获得足够高的信噪比（SNR)，并降低软件工作量。

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
 1 ///////ADC的初始化配置/////////
 2 void ADC_Config(void)
 3 {
 4   ADC_InitTypeDef ADC_InitStructure;//定义ADC结构体
 5   //DMA_InitTypeDef DMA_InitStructure;//定义DMA结构体  
 6   GPIO_InitTypeDef GPIO_InitStructure;
 7   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//使能DMA1时钟
 8   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1| RCC_APB2Periph_GPIOA, ENABLE ); //使能ADC1及GPIOA时钟
 9   /*作为ADC1的6通道模拟输入的GPIO初始化配置*/
10   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
11   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
12   GPIO_Init(GPIOA, &GPIO_InitStructure);
13   RCC_ADCCLKConfig(RCC_PCLK2_Div4);   //设置ADC分频因子4 56M/4=14,ADC最大时间不能超过14M
14   /*下面为ADC1的配置*/
15   ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1工作在独立模式
16   ADC_InitStructure.ADC_ScanConvMode = ENABLE;//模数转换工作在扫描模式（多通道）
17   ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//模数转换工作在连续模式
18   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由软件而不是外部触发启动
19   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC数据右对齐
20   ADC_InitStructure.ADC_NbrOfChannel = 1;//转换的ADC通道的数目为1
21   ADC_Init(ADC1, &ADC_InitStructure);//要把以下参数初始化ADC_InitStructure
22   //转换时间Tconv=采样时间+12.5个周期
23   ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_1Cycles5);
24   /*使能ADC1的DMA传输方式*/
25   ADC_DMACmd(ADC1, ENABLE);
26   /*使能ADC1 */
27   ADC_Cmd(ADC1, ENABLE);
28   /*重置ADC1的校准寄存器 */   
29   ADC_ResetCalibration(ADC1);
30   /*获取ADC重置校准寄存器的状态*/
31   while(ADC_GetResetCalibrationStatus(ADC1));
32   ADC_StartCalibration(ADC1); /*开始校准ADC1*/
33   while(ADC_GetCalibrationStatus(ADC1)); //等待校准完成
34   ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能ADC1软件转换
35 }
```

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 

ADC配置代码中值得注意的是：为了使A/D转换器获得1MSPS的采样率，STM32系统时钟被调整到56MHz，而非使用了常见72MHz。12位的STM32片上ADC最短需要14个ADC时钟周期（1.5个采样周期 + 12.5个逐次逼近周期）能完成一次转换，将系统时钟调整到56MHz就是为了在四分频后获得14MHz的ADC时钟，从而实现1MSPS的采样率（1us的采样间隔）。当然这也意味着，除采样和转换时间外，两次采样之间不容许再有其他的数据读写时间，只能由“DMA+硬件连续触发模式”来实现转换自动触发和结果自动存储。

另外，由于系统时钟、采样周期和逐次逼近转换所使用的ADC时钟数都无法连续调节，采用这种方式实现的ADC的采样率也就无法连续调节为任意需要的值。这也验证了使用 “DMA+硬件连续触发模式”的A/D转换模式无法使公式（1）中当K为整数的同时L为2的整数次幂的结论（5.1小节）。

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
 1 ///////ADC的DMA传输配置////
 2 void ADC_DMA_Config()
 3 {
 4   /*DMA1的通道1配置*/
 5   DMA_DeInit(DMA1_Channel1);
 6   DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//传输的源头地址
 7   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;//目标地址
 8   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //外设作源头
 9   DMA_InitStructure.DMA_BufferSize = 1024;//数据长度为1024，卷积算法用1000点，FFT用1024点
10   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址寄存器不递增
11   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址递增
12   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设传输以字节为单位
13   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//内存以字节为单位
14   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//不循环
15   DMA_InitStructure.DMA_Priority = DMA_Priority_High;//4优先级之一的(高优先)
16   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //非内存到内存
17   DMA_Init(DMA1_Channel1, &DMA_InitStructure);//根据以上参数初始化DMA_InitStructure
18   DMA_Cmd(DMA1_Channel1, ENABLE);//使能DMA1
21 }
```

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 

控制ADC的DMA的配置代码中值得注意的是：（1）由于A/D转换是“转换一笔数据就处理一次，下次转换就再次处理”，该DMA通道使用了不循环模式DMA_Mode_Normal。关于A/D转换器的触发时刻，请参见图5所示的程序整体流程图。（2）DMA存储的长度为1024次，而在DFT算法中使用了前1000次的结果，FFT算法中则使用了1024次的转换结果。

#### 2)自动输出波形的D/A转换的实现

信号重建也正好使用STM32F1上的两个D/A转换器分别产生A'和B'两个信号，它们的输出刷新率保持与A/D转换相同，同为1MSPS。D/A转换也只能使用DMA模式控制产生连续的模拟输出。相关的配置代码如下所示。

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
 1 ///////DAC1的DMA传输配置////
 2 void DAC1_DMA_Config(short* ROM_TBL)
 3 {
 4     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);//使能DMA2时钟
 5      //DMA2通道3配置 
 6     DMA_DeInit(DMA2_Channel3); //根据默认设置初始化DMA2
 7     DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_Address;//外设地址
 8     DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ROM_TBL;//内存地址
 9     DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//外设DAC作为数据传输的目的地
10     DMA_InitStructure.DMA_BufferSize =200;//数据长度
11     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址寄存器不递增
12     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址递增
13     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设传输以半字为单位
14     DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//内存以半字为单位
15     DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环模式
16     DMA_InitStructure.DMA_Priority = DMA_Priority_High;//4优先级之一的(高优先级)
17     DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//非内存到内存
18     DMA_Init(DMA2_Channel3, &DMA_InitStructure);//根据以上参数初始化DMA_InitStructure
19     //使能DAC的DMA中断
20   DMA_ITConfig(DMA2_Channel3, DMA_IT_TC, ENABLE);//配置控制DAC的DMA通道传输完成中断         
21     //使能DMA2的通道3
22     DMA_Cmd(DMA2_Channel3, ENABLE);    
23 }
```

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
///////DAC2的DMA传输配置////
void DAC2_DMA_Config(short* ROM_TBL)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);//使能DMA2时钟
     //DMA2通道4配置 
    DMA_DeInit(DMA2_Channel4); //根据默认设置初始化DMA2
    DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R2_Address;//外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ROM_TBL;//内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//外设DAC作为数据传输的目的地
    DMA_InitStructure.DMA_BufferSize =200;//数据长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址寄存器不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设传输以半字为单位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//内存以半字为单位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;//4优先级之一的(高优先级)
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//非内存到内存
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);//根据以上参数初始化DMA_InitStructure
    //使能DAC的DMA中断
  DMA_ITConfig(DMA2_Channel4, DMA_IT_TC, ENABLE);//配置控制DAC的DMA通道传输完成中断     
    //使能DMA2的通道4
    DMA_Cmd(DMA2_Channel4, ENABLE);    
}
```

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

值得注意的是：1）与A/D的DMA控制方式略有不同，以保证输出波形的连续性，D/A使用了DMA的循环地址模式。而A/D则只需要在进行波形频率、类型分析，以及间隔一段时间的相位调整时采集一帧信号即可，因此A/D转换使用了DMA的单次采样模式。2）D/A转换的转换速度较，因此D/A转换的间隔只能由TIM6和TIM7来定时实现。当然这也通过这两个定时器的操作来实现相位的实时校正做好了准备。DAC1和DAC2以及为它们定时的TIM6和TIM7的配置如下所示：

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
 1 void DAC_Config(void)
 2 {
 3     DAC_InitTypeDef  DAC_InitStructure;//库函数定义DAC结构体
 4     GPIO_InitTypeDef GPIO_InitStructure;//GPIO结构体
 5     RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//DAC时钟使能
 6     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);//使能定时器时钟
 7     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//使能定时器时钟
 8     /*将GPIO配置为DAC的模拟复用功能*/
 9     GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
10     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
11     GPIO_Init(GPIOA, &GPIO_InitStructure);
12   //////DAC通道1由基本定时器6触发////////////
13   /* TIM6配置*/
14   TIM_PrescalerConfig(TIM6,1-1,TIM_PSCReloadMode_Update);//设置TIM6预分频值
15   TIM_SetAutoreload(TIM6, 56-1);//设置定时器计数器值
16   /* TIM6触发模式选择，这里为定时器2溢出更新触发*/
17   TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
18   DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;//定时器6触发
19   DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;//无波形产生
20   DAC_InitStructure.DAC_OutputBuffer= DAC_OutputBuffer_Disable;//DAC_OutputBuffer_Enable;//不使能输出缓存
21   DAC_Init(DAC_Channel_1, &DAC_InitStructure);//根据以上参数初始化DAC结构体
22   //////DAC通道2由基本定时器7触发////////////
23   /* TIM7配置*/
24   TIM_PrescalerConfig(TIM7,1-1,TIM_PSCReloadMode_Update);//设置TIM7预分频值
25   TIM_SetAutoreload(TIM7, 56-1);//设置定时器计数器值
26   /* TIM7触发模式选择，这里为定时器2溢出更新触发*/
27   TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);
28   DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;//定时器7触发
29   DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;//无波形产生
30   DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;//DAC_OutputBuffer_Enable;//不使能输出缓存
31   /*DAC通道2配置*/
32   DAC_Init(DAC_Channel_2, &DAC_InitStructure);//根据以上参数初始化DAC结构体
33     /* 使能DAC通道1*/
34   DAC_Cmd(DAC_Channel_1, ENABLE);
35   //使能DAC通道1的DMA
36   DAC_DMACmd(DAC_Channel_1, ENABLE);
37     /* 使能DAC通道2*/
38   DAC_Cmd(DAC_Channel_2, ENABLE);
39   //使能DAC通道2的DMA
40   DAC_DMACmd(DAC_Channel_2, ENABLE);
41 //使能定时器6和7
42   TIM_Cmd(TIM6, ENABLE);
43   TIM_Cmd(TIM7, ENABLE);
44 }
```

[![复制代码](https://assets.cnblogs.com/images/copycode.gif)](javascript:void(0);)

 **本环节揭示出的嵌入式系统教学中的问题：**

现阶段本校的嵌入式系统课程教学，止步于让学生明白DMA是什么，而没有足够的学时让学生将一些重要的外设用熟、用透，导致学生的程序漏洞百出。我指导的一个很有希望的参赛队就是由于DMA控制的D/A输出在测试前临时失灵，造成现场测试失败。
对于嵌入式系统一类实践性较强的专业，只有通过课程设计才能让学生掌握所学内容的综合应用知识。而我校的课程设计近年来却完全从人才培养方案中删除，应该引起管理者的重视。 

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 9. 重建信号之间相位差的控制（发挥部分第（2）问） 

本题发挥部分的第（2）问要求参赛者实现对B’与A’的相位差进行控制，实现0~π的电位差。此处对“相位差”的定义如图2所示。 

对于本文所述的全数字算法，这个发挥部分没有太大难度。只需要在图5所示的流程中，根据题意，省去右侧循环中上半部分的A’信号相位校正代码，先做循环下部分的B’信号的相位校正后，再次在B’校正延迟时间的基础上再加上“相位-延迟时间”折算公式（8）式得到的延迟时间，就得到了A’信号的延迟时间。将延迟时间插入A’信号控制定时器TIM6的定时时间中，即可实现本题发挥部分第（2）问要求的信号间的相位控制。

![img](https://img2023.cnblogs.com/blog/1380455/202312/1380455-20231209151837150-475172918.png)

 

注：TB’为B’信号的周期，（8）式中我使用了弧度制，题干中却使用了角度制。

值得注意的是，发挥部分第（2）问对相位的分辨率的要求达到了5°，在100KHz下5°对应0.1389us，已经远小于重建信号D/A采样间隔的1us。只有直接调节重建信号D/A间隔时间的定时器才能达到要求的相位分辨率。具体方法参见本文第5小节第2部分“实时相位校正”中关于TIM6定时器延迟的代码。 

#### **本环节揭示出的嵌入式系统教学中的问题：**

有部分学生试图使用“正点原子”提供的STM32程序模板中的us级延时函数void delay_us(u32 nus);来实现（8）式的延迟。但这显然是行不通的：

a）该函数无法达到题目要求的0.1389us这样高的时间分辨率。

b）在us级别上该函数定时进度已经相当差劲，仅两次同样调用之间的时间抖动即可达到0.1us数量级；更别提调用该函数本身入栈和出账的耗时已经在0.1us级别！
这提示我们在未来的嵌入式系统教学中仍然要不遗余力的推进学生对于电流、电压和时间等物理量的感性认识，而示波器、信号发生器和频谱仪等常用仪器的普及应该是必要的先决条件。

 

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 10. 嵌入式程序开发中遇到的几个小问题及其解决办法 

#### 1) MDK的STM32程序模板中的堆和栈

 堆（heap）和栈（stack）本来是指对内存的使用方法，“栈”指先进先出，“堆”指用于实现优先队列的完全二叉树。但对编译器而言，它们通常用于指使用该方法的两个内存区域——堆指用完全二叉树管理，用于全局变量和malloc()等函数动态分配的内存区域；栈指用先进先出方式管理的，用于函数局部变量的内存区域。

由于STM32片上内存资源一般较少（我使用的STM32F103RC只有48KB RAM），一般分配给栈的区域尤其小。如，MDK的启动代码startup_stm32f10x_hd.s中缺省条件下就分配了512字节作为预留的Stack_Size，这在绝大多数情况下没有问题，因为很少有嵌入式函数会使用超过512字节的内存，但本题中的很多算法函数使用的内存数量远大于这个值，例如FFT函数使用内存数达到10KB以上。如果不对该配置进行修改，这些函数将对分配空间之外的内存区进行**无警告**的覆盖，从而造成程序莫名其妙出错。

配置方法很简单：在启动文件中进行如下修改即可：

```
1 Stack_Size      EQU     0x00004000 
```

#### 2) 相位校正环节中的延迟的实现

 本文第5节介绍了相位误差的校正算法及其实践，其方法是在计算得到输入信号中的A、B和重建信号A’、B’的相位差后，通过对A’、B’的时域延迟，实现相位对齐。赛后我看了很多同学的报告，他们都简单的采用了正点原子提供的STM32工程模板中的delay_us()来实现信号的时域延迟。这样做的问题在于：1）delay_us()延迟并不准确，由于中断和Flash指令读取缓冲等问题其误差可达20%以上。2）时间调节分辨率仅为1us，相当于一次A/D或D/A转换间隔，对100KHz的输入信号相当于36°相角，无法达到相位调节和锁定进度要求。

我的做法是直接在控制D/A转换的定时器（TIM6和TIM7）计数值基础上加延迟时间：这样做的好处是：1）定时器时间分辨率为系统时钟频率（本例为56MHz）的倒数，远远高于1us，从而实现更高进度的相位调节和锁定。2）直接对定时计数器中的计数值进行操作，可以避免调节代码本身执行时间带来的时间误差。 

**本环节揭示出的嵌入式系统教学中的问题：**

a) 学生缺乏复杂工程问题的练习。如果没有足够复杂的工程问题作为教学实例或练习题，这些问题占用内存有限，很难造成栈空间的溢出，也不会帮助学生掌握类似知识。

b) 电子电工相关专业的学生缺乏系统的计算机专业知识，即使了解问题所在，也很难从理论高度找到解决问题的办法。计算机知识向各行各业渗透的今天，作为相近专业的电子电工各类专业教学体系中应该渗透更多更深刻的计算机基础知识，为学生未来的发展打下宽口径的基础。

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 11. 测试结果

我用是德科技的DSOX6004A四通道示波器和普源精电的DS1022U两通道同步信号发生器进行了测试。结果令人满意。

1) 直接测试 

以下图片中，黄色和绿色的通道1&通道2是信号发生器产生的标准信号A和B，蓝色和红色的通道3&通道4则是嵌入式处理器重建的分离信号A’和B’。图10所示的是输入A、B分别为30KHz和60KHz的正弦波条件下的输出情况。采用通道1输入的30KHz信号作为示波器观察的触发信号，可以发现通道2输入的重建30KHz信号能够“连续稳定同频显示”（题目要求）。采用通道3输入的60KHz信号作为示波器观察的触发信号，可以发现通道4输入的重建60KHz信号也能够“连续稳定同频显示”。

注：有的双通道信号发生器产生两路具有倍数关系的信号时，这两路信号自己之间都没有同步关系，无法“连续稳定同频显示”。这可能是由于信号发生器内部的两个电路之间没有同步造成的，例如我用的是德科技的示波器DSOX6004A自带的两路信号发生器就无法做到同步输出，所以后面的测试中我没有使用DSOX6004A的两路信号发生功能，而是使用了专用的双通道信号发生器DS1022U。

当然使用这种两路信号不同步的信号发生器的并不影响本题的完成，因为参赛者不一定要像图10这样同时观察所有四路信号，而是可以分别观察A和A’，以及B和B’，并最终保证两对信号分别“连续稳定同频显示”即可算达到要求。

注：在示波器上观测到的输入信号和输出信号的反相现象，是由于模拟加法器采用了反相放大方式造成的。

![img](https://img2024.cnblogs.com/blog/1380455/202401/1380455-20240113153618394-108173384.png)

图10 A、B分别为30KHz和60KHz的正弦波时的输出 

下图所示的是输入为95KHz的较高频率下的输出。除输出信号相位略有抖动外，输出功能正常。 

随着频率增加，信号发生器产生的信号与STM32重建的信号频率之间的差异的绝对值虽然不变，但这个绝对误差相对于信号的周期却会相应增加。所以信号频率越高，肉眼观测到的抖动就会越明显。

如果想获得更佳的相位锁定效果，应降低两次相位调整之间的间隔，这需要更高的处理速度，可以考虑更高性能的MCU或FPGA。但个人觉得用FPGA实现本文所述的全数字算法的工作量较大，且涉及时序收敛等麻烦问题，需要较长的调试时间，已经超过了电子设计竞赛几天能够完成工作量的上限。

 

![img](https://img2024.cnblogs.com/blog/1380455/202401/1380455-20240113154044056-413766905.png)

图11 A为35KHz的正弦波时的输出 

 

 下图所示的是本文6.2小节所述的：“较高输入频率刚好等于较低输入频率3倍，且低频信号为三角波”的特殊情况，此时会产生低频信号三次谐波被高频信号“污染”的情况。我采用了5次谐波判断的方法，由下图可知，结果还是很理想的。

![img](https://img2024.cnblogs.com/blog/1380455/202401/1380455-20240113154204110-1503991226.png)

图12 较高输入频率为较低频率3倍时，信号类型识别结果 

#### 2) 用相位检测芯片评估系统性能

为验证系统的性能，我使用了模拟相位检测芯片AD8302对输入信号A和重建信号A’之间的相位差进行了实时检测。该项测试不属于大学生电子设计竞赛要求的内容，纯属我自己为了评估算法的性能，加之手头又刚好有一块康威科技的AD8302的测试/开发板，才做的自选内容。

AD8302的性能参数这里不再赘述，感兴趣的可以阅读ADI的手册[2]。对于本测试比较有用的是其“输入信号相位差<--->输出电压”之间的传递函数。

![img](https://img2024.cnblogs.com/blog/1380455/202401/1380455-20240118120240879-857404064.jpg)

![img](https://img2024.cnblogs.com/blog/1380455/202401/1380455-20240118114000402-1050771052.png)

图13 AD8302测试电路及其“输入信号相位差<--->输出电压”的传递函数

 当我**关闭**STM32中算法的相位自动调节功能时，把信号A和A’作为AD8302的两个输入，同时连接到示波器的通道1（黄色）和通道3（蓝色）；代表二者相位差的AD8302输出VPHS连接到示波器的通道2（绿色）。得到了下图的输出波形。

![img](https://img2024.cnblogs.com/blog/1380455/202401/1380455-20240118114131725-853001610.png)

图14 不进行相位实时调节功能时AD8302的输出

 

对比手册中的传递函数，可以得到结论：信号发生器输出的信号和STM32重建的信号具有非常接近和稳定的频率，但两者存在基本恒定的微小差异。从而导致了AD8302输出呈现出稳定的线性递增的相位差，因此示波器的绿色通道呈现出了类似图11所示的三角形输出，其周期约为周期约为600ms，表明A和未经相位实时调节的A’信号大约会在600ms左右会相差一个正弦周期。

当我**开启**STM32中算法的相位自动调节功能时，AD8302的输出如下图所示。注意：此时通道2（绿色）的纵向分辨率为200mV。

![img](https://img2024.cnblogs.com/blog/1380455/202401/1380455-20240118114254183-1337816414.png)

图15 算法进行相位实时调节过程中AD8302的输出

 

而通道4（红色）测试的是我特意设置的一个GPIO的输出，它在软件执行到特定的位置时被置高或置低，以方便调试时观察软件运行状态。（注：该管脚被我命名为**TST_PIN**，它被置高和置低的位置,我标注在图5所示的“程序整体流程图”中灰色的地方）

观察图15所示的相位检测芯片AD8302的输出可以得到以下结论：

（1）算法会在通道4（红色）所示方波的一个周期内进行一次相位自动调整。调整之前，由于A和A’之间频率的微小差异造成相位误差累计增加，体现为AD8302的相位误差输出（通道2，绿色）线性增加。每次调整后，相位误差回复到相同的水平。在时域观察就得到图10所示的波形，相位差基本不变，示波器能够“稳定不漂移”的显示原信号和重建信号。

（2）当相位调整越频繁，（即红色方波周期越短，目前约为200-250ms）时，相位差累计也就越小（即绿色锯齿波的幅度就越小，目前约为200mV），它所代表的相位波动也就越小。目前使用的STM32F1系列@56MHz，对两路信号的相位校正计算时间约为200-250ms。若想进一步提高计算速度，可以改用支持浮点运算的STM32F40X系列（注：至于如何在STM32F40X下打开浮点运算单元FPU，及其对浮点运算的加速能力评估，请参见鄙人的博文：[ARM DSP库CMSIS-DSP的使用——以STM32F4浮点FFT为例](https://www.cnblogs.com/helesheng/p/15671138.html)）。当然如果使用FPGA，在数据采集的同时实现DFT和相位估算，计算将不再成为问题，限制相位调整频率的将只有A/D采样的时长L而已。

（3）AD8302输出的相位差电压（绿色通道），除周期与红色方波同步的锯齿波外，显示曲线“较粗”。这是该电压信号线上耦合的高频信号，该高频信号频率与A信号相同。其成因很简单：AD8302是一只可以工作到2.7GHz射频相位检测器，其输出带宽大于100KHz。康威科技的这块开发板并未对相位输出VPHS进行滤波，当输入信号A和B的频率小于其输出带宽时，难免有部分输入信号耦合到输出。对于想尝试使用本文第2节所述的方案2的读者，务必注意这个问题。

[回到顶部](https://www.cnblogs.com/helesheng/p/17888312.html#_labelTop)

## 参考文献：

[1] 胡广书. 数字信号处理——理论、算法与实现. 2nd ed. Beijing: 清华大学出版社, 2003. 

[2] Analog Devices, “AD8302: LF–2.7 GHz RF/IF Gain and Phase Detector” Data Sheet, Rev. B,  Date of Publication: September 28, 2006, [Online]. Retrieved from: <https://www.analog.com/media/en/technical-documentation/data-sheets/ad8302.pdf> on January 18, 2024.

 

标签: [STM32](https://www.cnblogs.com/helesheng/tag/STM32/), [全国大学生电子设计竞赛](https://www.cnblogs.com/helesheng/tag/全国大学生电子设计竞赛/), [2023年H题](https://www.cnblogs.com/helesheng/tag/2023年H题/), [离散傅里叶变换](https://www.cnblogs.com/helesheng/tag/离散傅里叶变换/), [电子电工教学](https://www.cnblogs.com/helesheng/tag/电子电工教学/), [数字信号处理](https://www.cnblogs.com/helesheng/tag/数字信号处理/), [AD8302](https://www.cnblogs.com/helesheng/tag/AD8302/)