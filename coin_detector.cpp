#include <iostream> 
#include <string> 
#include "opencv2/opencv.hpp"

int main(int argc, char **argv)
{
    std::string filename = argv[1];
    cv::Mat imageIn = cv::imread(filename, cv::IMREAD_COLOR);

    if (!imageIn.data)
    {
        std::cout << "Error while opening file " << filename << std::endl;
        return 0; 
    }

    // Image used to draw the ellipses on 
    cv::Mat imgEllipse = imageIn.clone(); 

    // Initialize the count of all coins to 0
    int quarters = 0; 
    int nickels = 0; 
    int dimes = 0; 
    int pennys = 0; 

    // convert the image to grayscale
    cv::Mat imageGray;
    cv::cvtColor(imageIn, imageGray, cv::COLOR_BGR2GRAY);

    // normalize the image
    cv::Mat imageNormalized;
    cv::normalize(imageGray, imageNormalized, 0, 255, cv::NORM_MINMAX, CV_8UC1);

    // find the image edges
    cv::Mat imageEdges;
    const double cannyThreshold1 = 100;
    const double cannyThreshold2 = 200;
    const int cannyAperture = 3;
    cv::Canny(imageGray, imageEdges, cannyThreshold1, cannyThreshold2, cannyAperture);
    
    // erode and dilate the edges to remove noise
    int morphologySize = 1;
    cv::Mat edgesDilated;
    cv::dilate(imageEdges, edgesDilated, cv::Mat(), cv::Point(-1, -1), morphologySize);
    cv::Mat edgesEroded;
    cv::erode(edgesDilated, edgesEroded, cv::Mat(), cv::Point(-1, -1), morphologySize);
    
    // locate the image contours (after applying a threshold or canny)
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(edgesEroded, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    // fit ellipses to contours containing sufficient inliers
    std::vector<cv::RotatedRect> fittedEllipses(contours.size());

    for(int i = 0; i < contours.size(); i++)
    {
        // compute an ellipse only if the contour has more than 5 points (the minimum for ellipse fitting)
        if(contours.at(i).size() > 5)
        {
            fittedEllipses[i] = cv::fitEllipse(contours[i]);
        }
    }

    // draw the ellipses
    const int minEllipseInliers = 50;
    for(int i = 0; i < contours.size(); i++)
    {
        // draw any ellipse with sufficient inliers
        if(contours.at(i).size() > minEllipseInliers)
        {
            // Use countour area size to identify coins
            double area = contourArea(contours.at(i)); 

            // Ellipse is a quarter and should be green
            if(area >= 11000.00)
            {
                quarters++; 
                cv::Scalar color = cv::Scalar(0, 255, 0);
                cv::ellipse(imgEllipse, fittedEllipses[i], color, 2);
            }

            // Ellipse is a nickel and should be yellow
            else if(area >= 8000.00)
            {
                nickels++; 
                cv::Scalar color = cv::Scalar(0, 255, 255);
                cv::ellipse(imgEllipse, fittedEllipses[i], color, 2);
            }

            // Ellipse is a penny and should be red
            else if(area >= 7000.00)
            {
                pennys++; 
                cv::Scalar color = cv::Scalar(0, 0, 255);
                cv::ellipse(imgEllipse, fittedEllipses[i], color, 2);
            }
            
            // Ellipse is a dime and should be blue
            else
            {
                dimes++; 
                cv::Scalar color = cv::Scalar(255, 0, 0);
                cv::ellipse(imgEllipse, fittedEllipses[i], color, 2);
            }
            
        }
    }

    // Print the counts of each coin
    std::cout << "Penny - " << pennys << std::endl; 
    std::cout << "Nickel - " << nickels << std::endl; 
    std::cout << "Dime - " << dimes << std::endl; 
    std::cout << "Quarter - " << quarters << std::endl; 

    // Calculate the total amount 
    double total = (0.25 * quarters) + (0.05 * nickels) + (0.01 * pennys) + (0.10 * dimes); 
    std::cout << "Total - $" << total << std::endl; 

    cv::imshow("imageIn", imageIn);
    cv::imshow("imgEllipse", imgEllipse);
    cv::waitKey();
}