#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetLineWidth(2);
	ofEnableDepthTest();
	ofDisableArbTex();

	this->cap_size = cv::Size(720, 720);
	this->rect_size = 180;
	for (int x = 0; x < this->cap_size.width; x += this->rect_size) {

		for (int y = 0; y < this->cap_size.height; y += this->rect_size) {

			auto image = make_unique<ofImage>();
			image->allocate(this->rect_size, this->rect_size, OF_IMAGE_COLOR);
			cv::Mat frame = cv::Mat(cv::Size(image->getWidth(), image->getHeight()), CV_MAKETYPE(CV_8UC3, image->getPixels().getNumChannels()), image->getPixels().getData(), 0);
			cv::Rect rect = cv::Rect(x, y, this->rect_size, this->rect_size);

			this->rect_images.push_back(move(image));
			this->cv_rects.push_back(rect);
			this->rect_frames.push_back(frame);

			this->location_list.push_back(glm::vec3(x, y, 0));
		}
	}

	this->number_of_frames = 0;
	vector<string> file_path_list = {
		"D:\\MP4\\cat.mp4"
	};

	for (auto& file_path : file_path_list) {

		this->cap.open(file_path);
		int frame_count = this->cap.get(cv::CAP_PROP_FRAME_COUNT);
		int read_count = 0;
		for (int i = 0; i < frame_count; i++) {

			cv::Mat src, tmp;
			this->cap >> src;
			if (src.empty()) {

				continue;
			}

			cv::resize(src, tmp, this->cap_size);
			cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

			this->frame_list.push_back(tmp);
			read_count++;
		}

		this->number_of_frames += read_count;
	}

	for (auto& location : this->location_list) {

		vector<int> next_index = vector<int>();
		int index = -1;
		for (auto& other : this->location_list) {

			index++;
			if (location == other) { continue; }

			float distance = glm::distance(location, other);
			if (distance <= this->rect_size) {

				next_index.push_back(index);
			}
		}

		this->next_index_list.push_back(next_index);
	}

	for (int i = 0; i < this->rect_frames.size() - 1; i++) {

		this->actor_list.push_back(make_unique<Actor>(this->location_list, this->next_index_list, this->destination_list));
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_span = 15;
	int prev_index_size = 0;
	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}

}

//--------------------------------------------------------------
void ofApp::draw() {

	for (int i = 0; i < this->actor_list.size(); i++) {

		int frame_index = (ofGetFrameNum() / 2) % this->number_of_frames;

		cv::Mat tmp_box_image(this->frame_list[frame_index], this->cv_rects[i]);
		tmp_box_image.copyTo(this->rect_frames[i]);

		this->rect_images[i]->update();
		this->rect_images[i]->getTexture().bind();
		ofSetColor(255);
		ofFill();
		ofDrawBox(glm::vec3(this->actor_list[i]->getLocation().x + this->rect_size * 0.5, this->cap_size.height - this->actor_list[i]->getLocation().y - this->rect_size * 0.5, 0), this->rect_size, this->rect_size, 5);
		this->rect_images[i]->unbind();

		ofSetColor(0);
		ofNoFill();
		ofDrawBox(glm::vec3(this->actor_list[i]->getLocation().x + this->rect_size * 0.5, this->cap_size.height - this->actor_list[i]->getLocation().y - this->rect_size * 0.5, 0), this->rect_size, this->rect_size, 5);
	}
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}