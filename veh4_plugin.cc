#ifndef _VEHICLE_PLUGIN4_
#define _VEHICLE_PLUGIN4_

#include <functional>
#include <gazebo/common/common.hh>
#include <ignition/math/Vector3.hh>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <fstream>
#include <iostream>


namespace gazebo{
	// plugin to control a lidar sensor
	class VehiclePlugin4: public ModelPlugin{
		public: VehiclePlugin4(){}

		public: virtual void Load(physics::ModelPtr _model4, sdf::ElementPtr _sdf){
			std::cerr << "\nThe  plugin is attach to model[" <<_model4->GetName() << "]\n";
			std::cerr<<"\njoints number is"<< _model4->GetJointCount();
			if(_model4->GetJointCount()==0){
				std::cerr<< "Invalid joint count,  plugin not loaded\n";
				return;
			}

			this->model = _model4;
			// this->model->SetRelativePose(ignition::math::Pose3d(0,10,0));
			this->updateConnection = event::Events::ConnectWorldUpdateBegin(
				std::bind(&VehiclePlugin4::OnUpdate, this));

			//get lidar joint
			this->joint= _model4->GetJoints()[0];
			// Setup a P-controller, with a gain of 0.1.
			this->pid = common::PID(0.1,0,0);
			//apply p-controller to the joint
			this->model->GetJointController()->SetVelocityPID(
				this->joint->GetScopedName(),this->pid);

			double velocity = 0.0;  
			if(_sdf->HasElement("velocity"))
				velocity=_sdf->Get<double>("velocity");
			this->SetVelocity(velocity);


			this->node =transport::NodePtr(new transport::Node());

			#if GAZEBO_MAJOR_VERSION < 8
			this->node->Init(this->model->GetWorld()->GetName());
			#else
			this->node->Init(this->model->GetWorld()->Name());
			#endif
			// Create a topic name

			std::string topicName = "~/" + this->model->GetName() + "/vel_cmd";
			std::cerr<<topicName<<'\n';
			// Subscribe to the topic, and register a callback
			this->sub = this->node->Subscribe(topicName,
			   &VehiclePlugin4::OnMsg, this);

		}

		public:
		template <size_t N>
		void splitString(std::string (&arr)[N], std::string str)
		{
		    int n = 0;
		    std::istringstream iss(str);
		    for (auto it = std::istream_iterator<std::string>(iss); it != std::istream_iterator<std::string>() && n < N; ++it, ++n)
		        arr[n] = *it;
		}

		public:void SetVelocity(const double &_vel){
			this->model->GetJointController()->SetVelocityTarget(
				this->joint->GetScopedName(),_vel);

		}
		public:void OnUpdate(){
			double xx=this->speedx; 
			double yy=this->speedy; 
			// this->model->SetLinearVel(ignition::math::Vector3d(0,-3,0));
			this->model->SetLinearVel(ignition::math::Vector3d(xx,yy,0));

			double x,y,z;
			// gazebo::physics::ModelState mstate;
			// ignition::math::Pose3d pose = mstate.Pose();
			ignition::math::Pose3d pose = this->model->WorldPose();
			ignition::math::Vector3d speed = this->model->RelativeLinearVel();
			ignition::math::Vector3d accel = this->model->RelativeLinearAccel();

			// public:physics::State state;
			// std::cerr<<"###simmulation time:"<<state.GetSimTime()<<"pose of agent:###\n";

			std::ofstream myfile;
			myfile.open("state4.txt",std::ios_base::app);
			common::Time time;
			// double ttt = time.GetWallTime();
			myfile<<time.GetWallTime()<<'\t';
			myfile<<pose<<'\t';
			myfile<<speed<<'\t'<<accel<<'\n';
			myfile.close();
			// std::cerr<<"vehicle4:"<<'\t';
			std::cerr<<"time"<<time.GetWallTime()<<'\t';
			std::cerr<<"x,y,z,roll,pitch,yaw:"<<pose<<'\n';
			std::cerr<<"speed:"<<-speed<<'\t';
			std::cerr<<"accerlaration:"<<-accel<<'\n';
			std::cerr<<"============================\n";

			
			this->model->SetLinearVel(ignition::math::Vector3d(0,-speedy,0));
			// this->model->SetLinearAccel(ignition::math::Vector3d(0,0.5,0));
		}
		private:event::ConnectionPtr updateConnection;

		private: physics::ModelPtr model; 
		private:physics::JointPtr joint;
		private:common::PID pid; 
		private:transport::NodePtr node;
		private:transport::SubscriberPtr sub;
		public: double speedy=9, speedx=0,accy=0.15;
		public: double self_v=9,self_acc=0.15;
		public: double a1= 0.2;
		public:double v_desire=12.0;
		
		public: double d = 30;


		/// \brief Handle incoming message
		/// \param[in] _msg Repurpose a vector3 message. This function will
		/// only use the x component.
		
		private: void OnMsg(ConstVector3dPtr &_msg)
		{
			static double accumulated_d = 0;
			std::string result = "";
		    std::ifstream fin("./build/state3.txt");
		    // std::cout << "final line length: " ;
		    if(fin.is_open()) {
		        fin.seekg(0,std::ios_base::end);      //Start at end of file
		        char ch = ' ';                        //Init ch not equal to '\n'
		        while(ch != '\n'){
		            fin.seekg(-2,std::ios_base::cur); //Two steps back, this means we
		                                              //will NOT check the last character
		            if((int)fin.tellg() <= 0){        //If passed the start of the file,
		                fin.seekg(0);                 //this is the start of the line
		                break;
		            }
		            fin.get(ch);                      //Check the next character
		        }
		    }

	        std::getline(fin,result);
	        fin.close();

	        // std::cout << "final line length: " << result.size() <<std::endl;

	        // std::cout << "final line: " << result <<std::endl;
	        std::string arr[15];

	        splitString(arr, result);

	        double speed = std::stod(arr[9]);
	        double acc = std::stod(arr[13]);

	        // read self file states below:
	        std::string state3 = "";
		    std::ifstream fin2("./build/state4.txt");
		    // std::cout << "final line length: " ;
		    if(fin2.is_open()) {
		        fin2.seekg(0,std::ios_base::end);      //Start at end of file
		        char ch2 = ' ';                        //Init ch not equal to '\n'
		        while(ch2 != '\n'){
		            fin2.seekg(-2,std::ios_base::cur); //Two steps back, this means we
		                                              //will NOT check the last character
		            if((int)fin2.tellg() <= 0){        //If passed the start of the file,
		                fin2.seekg(0);                 //this is the start of the line
		                break;
		            }
		            fin2.get(ch2);                      //Check the next character
		        }
		    }

	        std::getline(fin,state3);
	        fin.close();
	        std::string state[15];

	        splitString(state, state3);

	        double self_v = std::stod(state[9]);
	        double self_acc = std::stod(state[13]);
	        //1st following vehicle
	        accumulated_d += self_v + 0.5 * self_acc; 

	        double distance = v_desire + d; 
	        double diff = distance - accumulated_d;
	        if(self_v<v_desire){
	        	double v1 = self_v +self_acc *1; 
	        	if(diff>5){
	        		double ast = diff*1.0 /pow((5+ (v1+0.5*self_acc)),2);
	        		a1 = ast *(1-(self_v*1.0/v_desire));

	        	}
	        	else{
	        		a1 = -self_acc*(pow((diff/5.0),2));
	        	}
	        	speedy = v1;
	        	accy = a1;
	        }

		 	this->speedy = _msg->x();
			// this->speedx = 0; //change speed on x dimension only 
			// std::cerr<< "x"<<this->speedx;
			// std::cerr <<"y" <<this->speedy;
		}
		

	};

	GZ_REGISTER_MODEL_PLUGIN(VehiclePlugin4);
}
#endif