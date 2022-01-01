% Matlab Code for OnSpeed AHRS. (C) Lenny Iszak & FlyOnspeed Inc.
% Fuses Baro, Airspeed, Accerometers and Gyros for a full attitude solution including inertial AOA.
% Not for commercial use. 
% Not flight tested, use at your own risk.
% Needs flight test based tuning.

tic
%bias not added yet
errorTable=zeros(length(timeStamp),4);
%for initGBiasCov =  0.0006 : 0.0001 : 0.0012
%    for processNoiseGBias = 0.00001 :0.001: 0.01
%    for IASsmoothing = 1 : 476
dt = 1/238;

g2mps = 9.80665;
g = 9.80665;
deg2rad=pi/180;
kts2mps=0.514444;

%Optimal parameters obtained via fmincon optimization.
%tuned for pich/roll accuracy
initStateCov= 14.9667;
initGBiasCov= 7.0264;
processNoiseState= 0.00085796;
processNoiseGBias= 2.6058e-08;
measNoiseAcc= 0.15477;
measNoiseAlpha=0.097379;%0.027379;  % fmincon optimized
lpfGyr= 0.74527;
lpfAcc= 0.28963;
lpfVa= 0.99885;
PaltExpSmoothing= 0.67428;
VSIExpSmoothing= 0.9889;
FwdAccExpSmoothing= 0.68213;

% %Optimal parameters obtained via GA optimization:
% initStateCov= 10.144;
% initGBiasCov= 6.5466;
% processNoiseState= 0.0066935;
% processNoiseGBias= 1.2408e-08;
% measNoiseAcc= 1.1887;
% lpfGyr= 0.71279;
% lpfAcc= 0.10564;
% lpfVa= 0.999;
% PaltExpSmoothing= 0.97867;
% VSIExpSmoothing= 0.97646;
% FwdAccExpSmoothing= 0.41927;
% measNoiseAlpha=0.027379; % fmincon optimized
% % optimized%Elapsed time:109421.2568 sec


%optimized for alpha via fmincon, all 12 parameters, RMSerror=0.9216 degrees
% initStateCov= 13.6131;
% initGBiasCov= 7.6194;
% processNoiseState= 0.0026648;
% processNoiseGBias= 1.0187e-08;
% measNoiseAcc= 0.27378;
% lpfGyr= 0.64033;
% lpfAcc= 0.24909;
% lpfVa= 0.999;
% PaltExpSmoothing= 0.27973;
% VSIExpSmoothing= 0.99271;
% FwdAccExpSmoothing= 0.98626;
% measNoiseAlpha= 0.080003;

%latest
% initStateCov= 18.9441;
% initGBiasCov= 9.3679;
% processNoiseState= 0.0027555;
% processNoiseGBias= 1.1909e-08;
% measNoiseAcc= 0.29766;
% lpfGyr= 0.68673;
% lpfAcc= 0.23191;
% lpfVa= 0.99814;
% PaltExpSmoothing= 0.22806;
% VSIExpSmoothing= 0.99002;
% FwdAccExpSmoothing= 0.98053;
% measNoiseAlpha= 0.08065;


        % Initialise covariance matrix
        P = diag([(ones(3,1)*initStateCov).^2; (ones(3,1)*initGBiasCov).^2]);

        % Process noise matrix
        Q = diag([(ones(3,1)*processNoiseState).^2; (ones(3,1)*processNoiseGBias).^2]);

        % Measurement noise matrix
        R = diag([(ones(3,1)*measNoiseAcc).^2;(ones(1,1)*measNoiseAlpha).^2]);

        % States (phi, theta)
        x = zeros(6, 1); % roll, pitch, gamma, rollratebias, pitchratebias, yawratebias 
        % x=[roll, pitch, gamma, rollratebias, pitchratebias, yawratebias]

%         x(4) = mean(RollRate(1:500).*deg2rad);
%         x(5) = mean(PitchRate(1:500).*deg2rad);
%         x(6) = mean(YawRate(1: 500).*deg2rad);

        % Arrays for plotting
        t = timeStamp;
        phihat   = zeros(1, length(t));
        thetahat = zeros(1, length(t));
        gammahat = zeros(1, length(t));

        errRoll  = 0.0;
        errPitch = 0.0;
        len=length(timeStamp);
        PaltSmoothedExp=zeros(len,1);
        FlightPathAngle=zeros(len,1);
        PaltSmoothedExp(1)=Palt(2);
        for n = 2 : len
             PaltSmoothedExp(n) = PaltExpSmoothing * PaltSmoothedExp(n-1) + (1 - PaltExpSmoothing) * Palt(n);
        end
                
        VSI=gradient(PaltSmoothedExp)./dt .* 0.3048;%m/s
        VSISmoothedExp=zeros(len,1);
        VSISmoothedExp(1)=0;
        for n = 2 : len
             VSISmoothedExp(n) = VSIExpSmoothing * VSISmoothedExp(n-1) + (1 - VSIExpSmoothing) * VSI(n);
        end
        
% %       
%         VertAccExpSmoothing=0.996;
%         VertAcc=gradient(VSISmoothedExp)./dt;
%         VertAccSmoothedExp=zeros(len,1);
%         VertAccSmoothedExp(1)=0;
%         for n = 2 : len
%              VertAccSmoothedExp(n) = VertAccExpSmoothing * VertAccSmoothedExp(n-1) + (1 - VertAccExpSmoothing) * VertAcc(n);
%         end        

        %FwdG correction
        IASsmoothed=zeros(len,1);
        IASsmoothed(1)=IAS(2);
        for n = 2 : len
             IASsmoothed(n) = lpfVa * IASsmoothed(n-1) + (1 - lpfVa) * IAS(n);
        end
        TAS=(IASsmoothed+IASsmoothed.*PaltSmoothedExp./1000.*0.02).*0.514444; % PAlt in feet here for +2% TAS rule of thumb approximation       
        FwdAcc=gradient(IAS)./dt.*0.514444;
        FwdAccExpSmoothed=zeros(len,1);
        FwdAccExpSmoothed(1)=0;
        for n = 2 : len
             FwdAccExpSmoothed(n) = FwdAccExpSmoothing * FwdAccExpSmoothed(n-1) + (1 - FwdAccExpSmoothing) * FwdAcc(n);
        end  
%                 %FwdG correction
%         IASdiff=gradient(IAS); %last value is missing, needs to be added
%         %IASdiff(length(IASdiff)+1) = IASdiff(length(IASdiff));
%         IASsmoothing=1;
%         IASdiffSmoothed=movavg(IASdiff,'simple',IASsmoothing); % first 50 values are NAN and need to be zeroed
%         IASdiffSmoothed(isnan(IASdiffSmoothed))=0;
%         
%         %aFwdCp=-IASdiffSmoothed .* 0.514444/dt .* 0.10197162129779;
          aFwdCp=-FwdAccExpSmoothed.* 0.10197162129779;
%         TAS=(IAS+IAS.*PaltSmoothedExp./1000.*0.02); % PAlt in feet here for +2% TAS rule of thumb approximation 

%calculate flightpath angle (radians)
        for n = 1 : len
           if IASsmoothed(n)>0
                VSIperIAS(n)=VSISmoothedExp(n)/(TAS(n));
                %VSIperIAS(n)=KalmanVSI(n)*0.00508/(TAS(n));
                if (VSIperIAS(n) >=-1) && (VSIperIAS(n) <=1)
                    FlightPathAngle(n) = asin(VSIperIAS(n));
                else
                    FlightPathAngle(n)=0;
                    VSIperIAS(n)=0;                    
                end;                
            else
                FlightPathAngle(n)=0;
                VSIperIAS(n)=0;                
            end;
        end

% box install error in radians
installPitch=-3.69 * 0.0174532925; % radians RV4
installRoll=-0.29 * 0.0174532925;
installYaw=0; 
installX=0;
installY=0;
installZ=0;
YawRateRad=YawRate .* 0.0174532925;
PitchRateRad=PitchRate .* 0.0174532925;
RollRateRad=RollRate.* 0.0174532925;

yawRateDiff=diff(YawRateRad);
rollRateDiff=diff(RollRateRad);
pitchRateDiff=diff(PitchRateRad);
yawRateDiff(length(yawRateDiff)+1) = yawRateDiff(length(yawRateDiff));
rollRateDiff(length(rollRateDiff)+1) = rollRateDiff(length(rollRateDiff));
pitchRateDiff(length(pitchRateDiff)+1) = pitchRateDiff(length(pitchRateDiff));


%forward Acc
ForwardGic=9.80665 .* ForwardG .* cos(installPitch) .* cos(installYaw) +9.80665 .* LateralG .* (sin(installRoll) .* sin(installPitch) .* cos(installYaw)- sin(installYaw) .* cos(installRoll))+ 9.80665 .* VerticalG .* ( cos(installYaw) .* cos(installRoll).* sin(installPitch)+ sin(installYaw).* sin(installRoll))+ ((YawRateRad .^2 + PitchRateRad .^2)  .* installX - (RollRateRad .* PitchRateRad-yawRateDiff ).* installY)-(RollRateRad .* YawRateRad+pitchRateDiff) .* installZ;
ForwardGic=ForwardGic .* 0.101972; % back to g

%lateral Acc

LateralGic=9.80665 .* ForwardG .* cos(installPitch) .* sin(installYaw) +9.80665 .* LateralG .* (sin(installYaw) .* sin(installPitch).* sin(installRoll) + cos(installYaw) .* cos(installRoll))+ 9.80665 .* VerticalG .* ( sin(installYaw) .* cos(installRoll).* sin(installPitch)- cos(installYaw).* sin(installRoll))- (RollRateRad .* PitchRateRad +yawRateDiff).* installX + (RollRateRad .^2 +YawRateRad .^2).* installY-(PitchRateRad .* YawRateRad-rollRateDiff) .* installZ;
LateralGic=LateralGic .* 0.101972; % back to g

%vertical Acc
VerticalGic=9.80665 .* -ForwardG .* sin(installPitch) +9.80665 .* LateralG .* sin(installRoll) .* cos(installPitch)+ 9.80665 .* VerticalG .* cos(installRoll) .* cos(installPitch) - (RollRateRad .* YawRateRad -pitchRateDiff).* installX - (PitchRateRad .* YawRateRad+rollRateDiff) .* installY +(RollRateRad .^2 +PitchRateRad .^2).* installZ;
VerticalGic=VerticalGic .* 0.101972; % back to 

        
%gyro corrections
        
%roll Gyro
RollRateRadic=RollRateRad .* cos(installPitch) .* cos(installYaw) + PitchRateRad .* ( cos(installYaw) .* sin(installRoll) .* sin(installPitch) .* - sin(installYaw) .* cos(installRoll) ) + YawRateRad .* ( cos(installYaw) .* cos(installRoll) .* sin(installPitch) + sin(installYaw) .* sin(installRoll));
RollRateDegic=RollRateRadic .* 57.2958; % back to degrees/sec

% pitch Gyro
PitchRateRadic=RollRateRad .* cos(installPitch) .* sin(installYaw) + PitchRateRad .* ( sin(installYaw) .* sin(installRoll) .* sin(installPitch) + cos(installYaw) .* cos(installRoll)) + YawRateRad .* ( sin(installYaw) .* cos(installRoll) .* sin(installPitch) - cos(installYaw) .* sin(installRoll));
PitchRateDegic=PitchRateRadic .* 57.2958; % back to degrees/sec

% yaw gyro
YawRateRadic= RollRateRad .* -sin(installPitch) + PitchRateRad .* sin(installRoll) .* cos(installPitch) + YawRateRad .* cos(installPitch) .* cos(installRoll);
YawRateDegic= YawRateRadic .* 57.2958; % back to degrees/sec        

        biasp = zeros(1, length(t));
        biasq = zeros(1, length(t));
        biasr = zeros(1, length(t));

        p = 0;
        q = 0;
        r = 0;
        ax = mean(ForwardGic(1:10)+aFwdCp(1:10)) .* g2mps;
        ay = mean(LateralGic(1:10)).* g2mps;
        az = mean(-VerticalGic(1:10)).* g2mps;
        Va = mean(TAS(1:10));

        for n = 1 : length(timeStamp)
            % Get measurements and low-pass filter them
            p = lpfGyr * p + (1 - lpfGyr) * -RollRateDegic(n) * deg2rad;
            q = lpfGyr * q + (1 - lpfGyr) * -PitchRateDegic(n) * deg2rad;
            r = lpfGyr * r + (1 - lpfGyr) * YawRateDegic(n) * deg2rad;

            ax = lpfAcc * ax + (1 - lpfAcc) * (ForwardGic(n)) * g2mps;
            ay = lpfAcc * ay + (1 - lpfAcc) * (LateralGic(n)) * g2mps;
            az = lpfAcc * az + (1 - lpfAcc) * -VerticalGic(n) * g2mps;

            %Va = lpfVa * Va + (1 - lpfVa) * IAS(n)*0.514444;
            Va= TAS(n);
            %Vsi=VSISmoothedExp(n); % // negative because z axis points down
            %Vsi=KalmanVerticalSpeed(n);
            %FwdAccAir=FwdAccExpSmoothed(n);
            %VertAccAir=VertAccSmoothedExp(n); % // negative because z axis points down
            gamma_measured=FlightPathAngle(n);
            
            % Extract states
            phi = x(1);
            theta = x(2);
            alpha= x(3);
            bp =  x(4);
            bq =  x(5);
            br =  x(6);
            

            xTimes=1;

        %    for it = 1 : xTimes

                % Compute trig terms
                sph = sin(phi); cph = cos(phi);
                tth = tan(theta);

                % State transition function, xdot = f(x, u)                
%                  f = [(p-bp) + (q-bq) * sph * tth + (r-br) * cph * tth; ...
%                      (q-bq) * cph - (r-br) * sph; 0; 0; 0];
                 f = [(p-bp) + (q-bq) * sph * tth + (r-br) * cph * tth; ...
                     (q-bq) * cph - (r-br) * sph; 0; 0; 0; 0];

                % Update state estimate
                x = x + (dt / xTimes) * f;

                % Re-extract states
                phi = x(1);
                theta = x(2);
                alpha = x(3);
                bp = x(4);
                bq = x(5);
                br = x(6);

                % Compute trig terms
                sph = sin(phi);   cph = cos(phi);
                sth = sin(theta); cth = cos(theta); tth = sth / cth;

                % Compute Jacobian of f, A(x, u)
% A=                 [ sin(phi)*tan(theta)*(br - r) - cos(phi)*tan(theta)*(bq - q), - cos(phi)*(br - r)*(tan(theta)^2 + 1) - sin(phi)*(bq - q)*(tan(theta)^2 + 1), -1, -sin(phi)*tan(theta), -cos(phi)*tan(theta);...
%                        cos(phi)*(br - r) + sin(phi)*(bq - q),                                                                             0,  0,            -cos(phi),             sin(phi);...
%                                                            0,                                                                             0,  0,                    0,                    0;...
%                                                            0,                                                                             0,  0,                    0,                    0;...
%                                                            0,                                                                             0,  0,                    0,                    0];
                A = [ sin(phi)*tan(theta)*(br - r) - cos(phi)*tan(theta)*(bq - q), - cos(phi)*(br - r)*(tan(theta)^2 + 1) - sin(phi)*(bq - q)*(tan(theta)^2 + 1), 0, -1, -sin(phi)*tan(theta), -cos(phi)*tan(theta);...
                                           cos(phi)*(br - r) + sin(phi)*(bq - q),                                                                             0, 0,  0,            -cos(phi),             sin(phi);...
                                                                               0,                                                                             0, 0,  0,                    0,                    0;...
                                                                               0,                                                                             0, 0,  0,                    0,                    0;...
                                                                               0,                                                                             0, 0,  0,                    0,                    0;...
                                                                               0,                                                                             0, 0,  0,                    0,                    0];
                                                                           
                % Update error covariance matrix
                P = P + dt * (A * P + P * A' + Q);   

        %    end

            % Compute accelerometer output estimates z(x, u)
                afwd=-aFwdCp(n)*g2mps;

                  z= [afwd+(q-bq) * Va * sin(theta - gamma_measured) + g * sin(theta);...
                      (r-br) * Va * cos(theta-gamma_measured) - (p-bp) * Va * sin(theta - gamma_measured) - g * cos(theta) * sin(phi);...
                     -(q-bq) * Va * cos(theta-gamma_measured) - g * cos(theta) * cos(phi);...
                      alpha];
                
            
            % Jacobian of z, C(x, u)         
%              C= [                      0,          g*cos(theta),             0, -Va*sin(theta-gamma_measured),              0;...
%                  -g*cos(phi)*cos(theta), g*sin(phi)*sin(theta), Va*sin(theta-gamma_measured),              0, -Va*cos(theta-gamma_measured);...
%                   g*cos(theta)*sin(phi), g*cos(phi)*sin(theta),             0,  Va*cos(theta-gamma_measured),              0];
                                 
               C= [                      0,          g*cos(theta), 0,             0, -Va*sin(theta-gamma_measured),              0;...
                   -g*cos(phi)*cos(theta), g*sin(phi)*sin(theta), 0, Va*sin(theta-gamma_measured),              0, -Va*cos(theta-gamma_measured);...
                    g*cos(theta)*sin(phi), g*cos(phi)*sin(theta), 0,             0,  Va*cos(theta-gamma_measured),              0;...
                                        0,                     0, 1,             0,              0,              0];                              
                                 

            % Kalman gain
            K = P * C' / (C * P * C' + R);

            % Update error covariance matrix
            P = (eye(length(x)) - K * C) * P;

            % Update state estimate using measurements (accelerometer)
            x = x + K * ([ax; ay; az;theta-gamma_measured] - z);

            % Store state estimates
            phihat(n)   = x(1) * 180 / pi;
            thetahat(n) = x(2) * 180 / pi;
            alphahat(n) = x(3) * 180 / pi;
            biasp(n) = x(4) * 180 / pi;
            biasq(n) = x(5) * 180 / pi;
            biasr(n) = x(6) * 180 / pi;



        end


%          errorTable(counter,1)=IASsmoothing;
%          errorTable(counter,2)=errRoll;
%          errorTable(counter,3)=errPitch;

        
        subplot(2, 1, 1)
        plot(t, vnRoll, t, phihat)
        title('Roll Angle')
        legend('True', 'Estimate')
        
        subplot(2, 1, 2)
        plot(t, vnPitch, t, thetahat)
        title('Pitch Angle')
        legend('True', 'Estimate')
        pitchError=vnPitch-(thetahat');
%    end
%end    
%    end
maxLen=374000; % skip the end of the flight for RMS calc
for n = 1 : maxLen
errAlpha(n)= sqrt((AngleofAttack(n) - alphahat(n)).^2);
errorRoll(n)=sqrt((vnRoll(n)-phihat(n)).^2);
errorPitch(n)=sqrt((vnPitch(n)-thetahat(n)).^2);
errAlpha(n)= sqrt((AngleofAttack(n) - alphahat(n)).^2);
end
KalRMS=sum(errAlpha)/maxLen+sum(errorPitch)/maxLen+0.32293*sum(errorRoll)/maxLen;
toc