%% Quadcopter Estimation
clear;clc;
% Parameter Declaration
g=9.81;
Ixx=0.01366;Iyy=0.01337;Izz=0.02648;g=9.81;
arm_length=0.27;d=arm_length/sqrt(2);m=0.903;
R_prop=0.13; rho_air=1.225;coeff_thrust=0.01295;
m_motor=0.052;r_motor=0.015;
Ir=(1/2)*m_motor*r_motor^2;%propeller inertia given by motor + propeller
% Continuous State Space Declaration: Model
K1=(d/Ixx)*sqrt(m*g*coeff_thrust*rho_air*pi*R_prop^4);
K2=(d/Iyy)*sqrt(m*g*coeff_thrust*rho_air*pi*R_prop^4);
A=zeros(4,4);A(2,1)=1;A(4,3)=1;
B=zeros(4,4);B(1,1:4)=K1*[1,-1,1,-1];B(3,1:4)=K2*[1,1,-1,-1];
B=5000*B; %scaling factor for esc -> motor
C=[0,1,0,0;0,0,0,1];D=[];
sysC=ss(A,B,C,D);
rank(ctrb(A,B)); %full rank of 9 means system controllable
rank(obsv(A,C));
lsim(sysC,10*rand(4,200),[0:0.05:10-0.05],rand(4,1)); %simulate 10s of random disturbances
% Discrete State Space Declaration
sysD=c2d(sysC,0.01,'zoh'); % assume clock speed of 50mS
%% LQR Design
Qc=eye(4);Qc(2,2)=100;Qc(4,4)=100;
Rc=1000*eye(4);
Kd=dlqr(sysD.A,sysD.B,Qc,Rc);
Kc=lqr(A,B,Qc,Rc);
sysC_control=ss(A-B*Kc,[],C,[]);
eig(sysC.A-sysC.B*Kc)
x0=[deg2rad(2),deg2rad(-3),deg2rad(1),deg2rad(4)];
initial(sysC_control,x0,0:0.1:10);
%%
sysD_control=ss(sysD.A-sysD.B*Kd,sysD.B,sysD.C,sysD.D,0.01);
eig(sysD_control.A)
[ysimd,tsimd,xsimd]=lsim(sysD_control,0.02*rand(4,1000),[0:0.01:10-0.01],rand(4,1));
lsim(sysD_control,0.02*rand(4,1000),[0:0.01:10-0.01],rand(4,1));
%% Kalman Filter Design
Qdist=0.001*eye(4);
Rnoise=0.000025*eye(2);
F=sysD.A;G=sysD.B;H=sysD.C;
xhat=[];Kalman_gain=[];Pk=[];
xrec=[];Krec=[];Prec=[];
xhat0=zeros(4,1);
P0=eye(4);
Pk_post=P0;
xhat_post=xhat0;

for n = 1:1000
    y(1,1:2) = ysimd(n,1:2);
    [Kk,xhat_pre,Pk_post]=kalman(F,G,H,y,Qdist,Rnoise,Pk_post,xhat_post,zeros(4,1));
    xrec=[xrec xhat_pre];
	Prec=[Prec trace(Pk_post)];
    Krec=[Krec sum(sum(Kk))];
end
plot(xrec(1,:));hold on;plot(ysimd(:,1));
% innovations=ysimd(1,:)'-H*xrec;
% plot(xcorr(detrend(innovations(1,:))))
%%
function [Kk,xhat_post,Pk_post]=kalman(F,G,H,y,Qdist,Rnoise,Pk_post,xhat_post,u)
   % time update
   Pk_pre = F*Pk_post*F'+Qdist;
   Kk = Pk_pre*H'/(H*Pk_pre*H'+Rnoise);
   xhat_pre = F*xhat_post+G*u;
   % measurement update
   xhat_post = xhat_pre + Kk*(y'-H*xhat_pre);
   Pk_post = (eye(4)-Kk*H)*Pk_pre*(eye(4)-Kk*H)'+Kk*Rnoise*Kk';
   % recursive update
   Pk_pre=Pk_post;
   xhat_pre=xhat_post;
end


