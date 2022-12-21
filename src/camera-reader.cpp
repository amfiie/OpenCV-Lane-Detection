#include "camera-reader.hpp"

CameraReader::CameraReader() {}

CameraReader::CameraReader(cv::VideoCapture&& cap) : cap(std::move(cap)) {}

CameraReader::~CameraReader() {
	running.clear();
	thread.join();
	cap.release();
}

void CameraReader::get_last_frame(cv::Mat& frame) {
	mutex.lock();
	cv::swap(frame_a, frame_b);
	mutex.unlock();
	frame = frame_b;
}

void CameraReader::start() {
	running.test_and_set();
	
	cap >> frame_a;
	
	thread = std::thread([this]() { 
		while (running.test_and_set()) {
			mutex.lock();
			// if cap is past last frame, reset it to beginning.
			if (!cap.read(frame_a)) {
				cap.set(cv::CAP_PROP_POS_FRAMES, 0);
				cap >> frame_a;
			}
			mutex.unlock();

			// Sleep the thread so that other thread can aquire mutex.
			// Since this thread still has to wait for another frame from the camera
			//   this is fine.
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	});
}

void create_camera_reader(CameraReader& reader, const std::string& filename) {
    cv::VideoCapture capture;
    if (filename.empty()) {
        const std::string CAM_SET = "libcamerasrc ! capsfilter caps=video/x-raw,width=640,height=480,framerate=20/1 ! appsink";
        capture = cv::VideoCapture(CAM_SET, cv::CAP_GSTREAMER);
    } else {
        const std::string CAM_SET = "res/" + filename;
        capture = cv::VideoCapture(VID_SET);
    }
	
	// if not success, exit program
	if (!capture.isOpened()) {
		std::cout << "ERROR: can't create capture!" << std::endl;
		exit(1);
	}

	new (&reader) CameraReader(std::move(capture));
}
