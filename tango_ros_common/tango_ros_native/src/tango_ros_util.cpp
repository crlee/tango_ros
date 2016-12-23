// Copyright 2016 Intermodalics All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "tango_ros_native/tango_ros_util.h"

#include <string.h>

#include <glog/logging.h>

namespace tango_ros_util {
bool InitRos(const char* master_uri, const char* host_ip,
                                   const char* node_name) {
  int argc = 3;
  char* master_uri_copy = strdup(master_uri);
  char* host_ip_copy = strdup(host_ip);
  char* argv[] = {"nothing_important" , master_uri_copy, host_ip_copy};

  LOG(INFO) << "\nMaster: " << master_uri_copy << "\n"
            << "Host: " << host_ip_copy << "\n"
            << "Node: " << node_name;
  ros::init(argc, &argv[0], node_name);
  free(master_uri_copy);
  free(host_ip_copy);
  if (ros::master::check()) {
    LOG(INFO) << "ROS MASTER IS UP! ";
  } else {
    LOG(ERROR) << "NO ROS MASTER! ";
    return false;
  }
  return true;
}

TangoRosNodeExecutor::TangoRosNodeExecutor() {}

TangoRosNodeExecutor::~TangoRosNodeExecutor() {}


int TangoRosNodeExecutor::Execute(const char* master_uri, const char* host_ip,
                                   const char* node_name) {
  if (InitRos(master_uri, host_ip, node_name)) {
    tango_ros_node_.reset(new tango_ros_native::TangoRosNode());
    if (tango_ros_node_->OnTangoServiceConnected()) {
      tango_ros_node_->StartPublishing();
      ros::Rate loop_rate(30);
      while(ros::ok()) {
        ros::spinOnce();
        loop_rate.sleep();
      }
    } else {
      LOG(ERROR) << "Could not connect to the Tango service.";
      return -1;
    }
  } else {
    LOG(ERROR) << "Could not init ROS.";
    return -2;
  }
  return 0;
}

void TangoRosNodeExecutor::Shutdown() {
  tango_ros_node_->StopPublishing();
  tango_ros_node_->TangoDisconnect();
}

void TangoRosNodeExecutor::UpdatePublisherConfiguration(bool publish_device_pose,
                                                        bool publish_point_cloud,
                                                        uint32_t publish_camera) {
  tango_ros_node_->UpdatePublisherConfiguration(publish_device_pose, publish_point_cloud, publish_camera);
}
}  // namespace tango_ros_util
