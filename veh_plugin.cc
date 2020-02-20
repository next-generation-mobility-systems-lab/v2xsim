#ifndef _VEHICLE_PLUGIN3_
#define _VEHICLE_PLUGIN3_

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
	class VehiclePlugin3: public ModelPlugin{
		public: VehiclePlugin3(){}

		public: virtual void Load(physics::ModelPtr _model2, sdf::ElementPtr _sdf){
			std::cerr << "\nThe  plugin is attach to model[" <<_model2->GetName() << "]\n";
			std::cerr<<"\njoints number is"<< _model2->GetJointCount();
			if(_model2->GetJointCount()==0){
				std::cerr<< "Invalid joint count, Velodyne plugin not loaded\n";
				return;
			}

			this->model = _model2;
			// this->model->SetRelativePose(ignition::math::Pose3d(0,10,0));
			this->updateConnection = event::Events::ConnectWorldUpdateBegin(
				std::bind(&VehiclePlugin3::OnUpdate, this));

			//get lidar joint
			this->joint= _model2->GetJoints()[0];
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
			   &VehiclePlugin3::OnMsg, this);

		}
		public:void SetVelocity(const double &_vel){
			this->model->GetJointController()->SetVelocityTarget(
				this->joint->GetScopedName(),_vel);

		}

		

		public:void OnUpdate(){
			// this->model->SetLinearVel(_msg);
			//set speed component
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
			myfile.open("state.txt",std::ios_base::app);
			common::Time time;
			// double ttt = time.GetWallTime();
			myfile<<time.GetWallTime()<<'\t';
			myfile<<pose<<'\t';
			myfile<<speed<<'\t'<<accel<<'\n';
			// std::cerr<<"leading vehicle:"<<'\t';
			std::cerr<<"time"<<time.GetWallTime()<<'\t';
			std::cerr<<"x,y,z,roll,pitch,yaw:"<<pose<<'\n';
			std::cerr<<"speed:"<<speed<<'\t';
			std::cerr<<"accerlaration:"<<accel<<'\n';
			std::cerr<<"============================\n";
			
			
			this->model->SetLinearVel(ignition::math::Vector3d(0,-12,0));
			// this->model->SetLinearAccel(ignition::math::Vector3d(0,0.5,0));
		}

		/// \brief Handle incoming message
		/// \param[in] _msg Repurpose a vector3 message. This function will
		/// only use the x component.
		
		private: void OnMsg(ConstVector3dPtr &_msg)
		{
			// this->SetVelocity(_msg->x());
		 	this->speedy = _msg->x();
		 	this->speedx = 0; //change speed on y dimension only 
			// std::cerr<< "x"<<this->speedx;
			// std::cerr <<"y" <<this->speedy;
		}



		/////////////// param 
		public: double speedy=-12.00, speedx=0;
		public: double cav_speedx=-0, cav_speedy=0;
		// public:double cav_positionx = 0.0, cav_positiony=0.0;
		///////////////////////////////////////
		private:event::ConnectionPtr updateConnection;

		private: physics::ModelPtr model; 
		private:physics::JointPtr joint;
		private:common::PID pid; 
		private:transport::NodePtr node;
		private:transport::SubscriberPtr sub;

	};

	GZ_REGISTER_MODEL_PLUGIN(VehiclePlugin3);
}
#endif