## IMU

* SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
  * Internal sample rate = 1khz

* -3db bandwidth
* Noise bandwidth





实验：

* 利用Data Ready 中断实现实时采样和定周期采样

* 采样绘时域图， Matlab FFT 分析频谱

* 静止状态滤波， visualization
* 上电自动零点校准（if静止） + 识别静止/运动（threshold）
  - Collect zero-bias to be subtracted. 
* Gimbal 平台 运动状态滤波

