/*********************************************************************
 * Copyright (c) 2019, SoftBank Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Softbank Corp. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ********************************************************************/

/** NOTE *************************************************************
 * This program had been developed by Michael T. Boulet at MIT under
 * the BSD 3-clause License until Dec. 2016. Since Nov. 2019, Softbank
 * Corp. takes over development as new packages.
 ********************************************************************/

// TODO: Migrate to ROS2

#ifndef VESC_DRIVER_VESC_DRIVER_H_
#define VESC_DRIVER_VESC_DRIVER_H_

#include <cassert>
#include <cmath>
#include <functional>
#include <memory>
#include <sstream>
#include <string>

#include <boost/optional.hpp>

#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <vesc_msgs/VescStateStamped.h>

#include "vesc_driver/vesc_interface.h"
#include "vesc_driver/vesc_packet.h"

namespace vesc_driver
{
class VescDriver
{
public:
  VescDriver(ros::NodeHandle nh, ros::NodeHandle private_nh);

private:
  // interface to the VESC
  VescInterface vesc_;
  void vescPacketCallback(const std::shared_ptr<VescPacket const>& packet);
  void vescErrorCallback(const std::string& error);

  // limits on VESC commands
  struct CommandLimit
  {
    CommandLimit(const ros::NodeHandle& nh, const std::string& str,
                 const boost::optional<double>& min_lower = boost::optional<double>(),
                 const boost::optional<double>& max_upper = boost::optional<double>());
    double clip(double value);
    std::string name;
    boost::optional<double> lower;
    boost::optional<double> upper;
  };
  CommandLimit duty_cycle_limit_;
  CommandLimit current_limit_;
  CommandLimit brake_limit_;
  CommandLimit speed_limit_;
  CommandLimit position_limit_;
  CommandLimit servo_limit_;

  // ROS services
  ros::Publisher state_pub_;
  ros::Publisher servo_sensor_pub_;
  ros::Subscriber duty_cycle_sub_;
  ros::Subscriber current_sub_;
  ros::Subscriber brake_sub_;
  ros::Subscriber speed_sub_;
  ros::Subscriber position_sub_;
  ros::Subscriber servo_sub_;
  ros::Timer timer_;

  // driver modes (possible states)
  typedef enum
  {
    MODE_INITIALIZING,
    MODE_OPERATING
  } driver_mode_t;

  // other variables
  driver_mode_t driver_mode_;  ///< driver state machine mode (state)
  int fw_version_major_;       ///< firmware major version reported by vesc
  int fw_version_minor_;       ///< firmware minor version reported by vesc
  int num_motor_pole_pairs_;   // the number of motor pole pairs

  // ROS callbacks
  void timerCallback(const ros::TimerEvent& event);
  void dutyCycleCallback(const std_msgs::Float64::ConstPtr& duty_cycle);
  void currentCallback(const std_msgs::Float64::ConstPtr& current);
  void brakeCallback(const std_msgs::Float64::ConstPtr& brake);
  void speedCallback(const std_msgs::Float64::ConstPtr& speed);
  void positionCallback(const std_msgs::Float64::ConstPtr& position);
  void servoCallback(const std_msgs::Float64::ConstPtr& servo);
};

}  // namespace vesc_driver

#endif  // VESC_DRIVER_VESC_DRIVER_H_
