#include "voxblox_ros_interface/voxblox_bag_importer.h"

int main(int argc, char** argv) {
  ros::init(argc, argv, "voxblox_bag_importer");
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InstallFailureSignalHandler();
  ros::NodeHandle nh;
  ros::NodeHandle nh_private("~");

  voxblox::VoxbloxBagImporter node(nh, nh_private);

  // Get paths as ros params...
  // Can just as easily be gflags or input params, really doesn't matter.
  std::string map_path, calibration_path, bag_path, pointcloud_topic;
  int integrate_every_nth_message = 1;
  nh_private.param("map_path", map_path, map_path);
  nh_private.param("calibration_path", calibration_path, calibration_path);
  nh_private.param("bag_path", bag_path, bag_path);
  nh_private.param("pointcloud_topic", pointcloud_topic, pointcloud_topic);
  nh_private.param("integrate_every_nth_message", integrate_every_nth_message,
                   integrate_every_nth_message);
  node.setSubsampling(integrate_every_nth_message);

  if (!node.setupRosbag(bag_path, pointcloud_topic)) {
    ROS_ERROR_STREAM("Couldn't open bag " << bag_path
                                          << " with pointcloud topic "
                                          << pointcloud_topic);
    ros::shutdown();
    return 0;
  }

  if (!node.setupMap(map_path)) {
    ROS_ERROR_STREAM("Couldn't open map " << map_path << "!");
    ros::shutdown();
    return 0;
  }

  if (!node.setupSensor(calibration_path)) {
    ROS_ERROR_STREAM("Couldn't set up sensor from calibration file "
                     << calibration_path);
    ros::shutdown();
    return 0;
  }

  ROS_INFO("Starting integrating data...");
  node.run();
  ROS_INFO("Finished integrating data! Integrated %llu messages out of %llu.",
           node.numMessages() / node.getSubsampling(), node.numMessages());

  node.visualize();

  ros::spin();
  return 0;
}
