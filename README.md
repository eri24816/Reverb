# Reverb 組開發紀錄

## Overview
我們要先做做看 jos 上的 Zita-Rev1

![](https://i.imgur.com/11HXws1.png)
https://ccrma.stanford.edu/~jos/pasp/Zita_Rev1.html#15833

* in_delay : 訊號一進來的delay
* distrub : 8*2 矩陣，模擬往8個方向跑的聲音
* allpass_comb : 應該是用來模擬撞到牆壁的聲音變化
* feedbackmatrix : 撞牆之後每條聲音又會分成8個方向。要注意它的 eigenvalue 不能超過1，系統才會 stable。
* delayfilters : 用來模擬在空氣中傳播的衰減
* fbdelaylines : 每個方向的聲音各自都要花一段時間才會再撞牆一次

這些運算中只有 allpass_comb、fbdelaylines 和 delayfilters 要另外設計，其它都是單純的運算。

## Matrix.h, Matrix.cpp
一些矩陣運算會用到的東西(一半是從網路上抄的)
```cpp=
//Matrix.h
struct Matrix;
Matrix inverse(Matrix matrix);
Matrix mult(Matrix& a, Matrix& b);
float* mult(Matrix m, float* v, float* result);
Matrix diag(float a[],int size);
```

## IIR.h
### class Module
Reverb 效果器中有很多運算步驟(上圖的橘色方塊)。有些步驟和時間有關(含有delay)，這些步驟被寫成class，繼承```Module```。一個裡```Module```可以包含更多```Module```。

在每個 time step，```Module::update```會拿到輸入的 vector (資料型態為float array)，處理後，輸出一個 vector。 
```cpp=
class Module {
public:
    int inputDim = 2;
    virtual float* update(float* input) = 0;
};
```
### class Delay : Module
用 ```std::queue``` 做的 delay line。每個 channel 可以設定 delay 不同的 sample 數。
### class Reverb : Module
Reverb 就是要拿去給 PluginProcesser 用的 reverb 效果。他是最大的 Module。

以下是一個較簡單的 reverb 模型。
```cpp=
// IIR system that simulates reverb
class Reverb : Module {
    Delay inDelay,fbDelayLine;
    float feedBack[2]{0,0};
public:
    Reverb():inDelay(2, new int[]{100,100}), fbDelayLine(2, new int[] {4000, 4000 }) {}

    // PluginProcessor calls this
    float* update(float* input) override{

        input = inDelay.update(input);
        add(2,input, feedBack);
        float* output = fbDelayLine.update(mult(2,input,0.8f));
        copy(2,feedBack, output);

        return output;
    }
};
```
### feedbackmatrix
它是 8x8 的 matrix。老師說它的 eigenvalue 不能超過1，系統才會 stable，但是也不能太小，否則聲音會太快消失。

設計出具有指定的 eigenvalue 和 eigenvector 的 matrix 的方法是
$$M=VDV^{-1}$$
，其中 $M$ 是我們要的 matrix，$V$ 是指定的 eigenvectors 疊起來形成的matrix，$D$ 是 diagonal matrix ，對角值為指定的 eigenvalues。
https://math.stackexchange.com/questions/54818/construct-matrix-given-eigenvalues-and-eigenvectors

所以，只要把 $D$ 的對角值設成0.9多，$V$ 隨便設定，$M$ 就會符合 feedbackmatrix 需要的條件了。

用 wolframalpha 驗算這個方法，看起來這個方法有用。
![](https://i.imgur.com/uajsCD6.png)

