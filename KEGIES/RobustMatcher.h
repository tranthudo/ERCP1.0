#pragma once
class RobustMatcher
{
public:
	
	~RobustMatcher(void);
private:

	// pointer to the feature point detector object
	cv::Ptr<cv::FeatureDetector> detector;
	// pointer to the feature descriptor extractor object
	cv::Ptr<cv::DescriptorExtractor> extractor;
	float ratio; // max ratio between 1st and 2nd NN
	bool refineF; // if true will refine the F matrix
	double distance; // min distance to epipolar
	double confidence; // confidence level (probability)

public:

	RobustMatcher();

	// Set the feature detector
	void setFeatureDetector(cv::Ptr<cv::FeatureDetector>& detect);

	// Set descriptor extractor
	void setDescriptorExtractor(cv::Ptr<cv::DescriptorExtractor>& desc);

	// Set the minimum distance to epipolar in RANSAC
	void setMinDistanceToEpipolar(double d);

	// Set confidence level in RANSAC
	void setConfidenceLevel(double c);

	// Set the NN ratio
	void setRatio(float r);

	// if you want the F matrix to be recalculated
	void refineFundamental(bool flag);

	// Clear matches for which NN ratio is > than threshold
	// return the number of removed points 
	// (corresponding entries being cleared, i.e. size will be 0)
	int ratioTest(std::vector<std::vector<cv::DMatch>>& matches);

	// Insert symmetrical matches in symMatches vector
	void symmetryTest(const std::vector<std::vector<cv::DMatch>>& matches1,
		const std::vector<std::vector<cv::DMatch>>& matches2,
		std::vector<cv::DMatch>& symMatches);

	// Identify good matches using RANSAC
	// Return fundemental matrix
	cv::Mat ransacTest(const std::vector<cv::DMatch>& matches,
		const std::vector<cv::KeyPoint>& keypoints1, 
		const std::vector<cv::KeyPoint>& keypoints2,
		std::vector<cv::DMatch>& outMatches);

	// Match feature points using symmetry test and RANSAC
	// returns fundemental matrix
	cv::Mat match(cv::Mat& image1, cv::Mat& image2, // input images 
		std::vector<cv::DMatch>& matches, // output matches and keypoints
		std::vector<cv::KeyPoint>& keypoints1, std::vector<cv::KeyPoint>& keypoints2);
};

