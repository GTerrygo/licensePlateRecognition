# licensePlateRecognition
  This application can recoginze plate number based on C++ and opencv. Using MFC to build a simple opearation UI.
  
# version
 vs 2017
 
 opencv 2.4
 
# Files structure
UIPR - the project of plate recoginze app.

SVMPR - the project of using svm to train data.

# Run
In the UIPR, running the project in VS and a MFC UI will show whcin consists of four components:

1. load image button and image display regin.
2. load SVM1 button - load char_svm.xml (to recognize plate number).
3. load SVM2 button - load plate_svm.xml (to recognize a region if it is a plate).
4. search button - find plate region in an image.
5. recognize button - recognize number in the plate region.

In the SVMPR, running the project in VS and training data with svm:

1. unzip data.rar to get training images.
2. in the main app:

  (1) type = 0 - train character and number images.
      type = 1 - train plate images.
      
  (2) train_path - 
  
            .\\data\\data_char\\train\\ (character and number images).
            .\\data\\data_Plate\\train_images\\ (plate images).
            
  (3) test_path -
            .\\data\\data_char\\test\\ (character and number images).
            .\\data\\data_Plate\\test_images\\ (plate images).  
            
  (4) out_name - the name of final trained xml file.
  
  
  # End
  Thanks the [blogger](https://www.cnblogs.com/subconscious/) to share his ideas.

