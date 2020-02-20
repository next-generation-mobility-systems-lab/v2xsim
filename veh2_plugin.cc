#ifndef _VEHICLE_PLUGIN_
#define _VEHICLE_PLUGIN_

#include <functional>
#include <gazebo/common/common.hh>
#include <ignition/math/Vector3.hh>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>

namespace gazebo{
	// plugin to control a lidar sensor
	class VehiclePlugin: public ModelPlugin{
		public: VehiclePlugin(){}

		public: virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf){
			std::cerr << "\nThe  plugin is attach to model[" <<_model->GetName() << "]\n";
			std::cerr<<"\njoints number is"<< _model->GetJointCount()<<'\n';
			if(_model->GetJointCount()==0){
				std::cerr<< "Invalid joint count, Velodyne plugin not loaded\n";
				return;
			}

			this->model = _model;
			// this->model->SetRelativePose(ignition::math::Pose3d(0,10,0));
			this->updateConnection = event::Events::ConnectWorldUpdateBegin(
				std::bind(&VehiclePlugin::OnUpdate, this));

			//get lidar joint
			this->joint= _model->GetJoints()[0];
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
			   &VehiclePlugin::OnMsg, this);

		}
		public:void SetVelocity(const double &_vel){
			this->model->GetJointController()->SetVelocityTarget(
				this->joint->GetScopedName(),_vel);

		}
		public:void OnUpdate(){
			this->model->SetLinearVel(ignition::math::Vector3d(0,-14,0));
			// this->model->SetLinearAccel(ignition::math::Vector3d(0,0.5,0));
		}

		/// \brief Handle incoming message
		/// \param[in] _msg Repurpose a vector3 message. This function will
		/// only use the x component.
		
		private: void OnMsg(ConstVector3dPtr &_msg)
		{
		  this->SetVelocity(_msg->x());
		}

		private:event::ConnectionPtr updateConnection;

		private: physics::ModelPtr model; 
		private:physics::JointPtr joint;
		private:common::PID pid; 
		private:transport::NodePtr node;
		private:transport::SubscriberPtr sub;

	};

	GZ_REGISTER_MODEL_PLUGIN(VehiclePlugin);
}
#endif