# Otoscope AI Model

A deep learning model for automated ear condition classification using otoscope images. This project uses transfer learning with MobileNetV2 to classify ear conditions into four categories: Chronic Otitis Media, Earwax Plug, Myringosclerosis, and Normal.


## 🔍 Overview

This project implements a convolutional neural network (CNN) using TensorFlow/Keras to classify otoscope images and assist in medical diagnosis. The model achieves 87% test accuracy using transfer learning from MobileNetV2.

![Otoscope Sample](https://github.com/user-attachments/assets/411b9b04-a0a5-4e3c-8aa3-0a95fbddf9b8)

## 📊 Dataset Classes

The model classifies ear conditions into four categories:

- **Chronic Otitis Media** (Class 0)
- **Earwax Plug** (Class 1) 
- **Myringosclerosis** (Class 2)
- **Normal** (Class 3)

### Dataset Statistics
- **Training Images**: 576 images
- **Validation Images**: 144 images  
- **Test Images**: 160 images
- **Total**: 880 images across 4 classes

## 🚀 Features

- **Transfer Learning**: Uses pre-trained MobileNetV2 as feature extractor
- **Data Augmentation**: Implements rotation, zoom, shift, shear, and flip augmentations
- **Model Performance**: Achieves 87% test accuracy
- **Inference Pipeline**: Complete prediction pipeline with visualization
- **Lightweight Architecture**: Optimized for deployment with MobileNetV2

## 🛠️ Requirements

```python
tensorflow>=2.0.0
matplotlib
numpy
```

## 📁 Project Structure

```
Otoscope_AIModel/
├── Dados/
│   ├── Training-validation/
│   │   ├── Chronic otitis media/
│   │   ├── Earwax plug/
│   │   ├── Myringosclerosis/
│   │   └── Normal/
│   └── Testing/
│       ├── Chronic otitis media/
│       ├── Earwax plug/
│       ├── Myringosclerosis/
│       └── Normal/
├── my_model.keras
└── train_model.py
```

## 🔧 Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/otoscope-ai-model.git
cd otoscope-ai-model
```

2. Install dependencies:
```bash
pip install tensorflow matplotlib numpy
```

3. Organize your dataset according to the project structure above.

## 💻 Usage

### Training the Model

```python
import tensorflow as tf
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.applications import MobileNetV2
from tensorflow.keras.models import Model
from tensorflow.keras.layers import GlobalAveragePooling2D, Dense, Dropout
from tensorflow.keras.optimizers import Adam

# Set your data paths
trainval_dir = "path/to/Training-validation"
test_dir = "path/to/Testing"

# Data augmentation and preprocessing
trainval_datagen = ImageDataGenerator(
    rescale=1./255, 
    validation_split=0.2, 
    rotation_range=20,
    zoom_range=0.15,
    width_shift_range=0.2,
    height_shift_range=0.2,
    shear_range=0.15,
    horizontal_flip=True
)

# Load data
train_data = trainval_datagen.flow_from_directory(
    trainval_dir,
    target_size=(224, 224),
    batch_size=32,
    class_mode='categorical',
    subset='training',
    shuffle=True
)

val_data = trainval_datagen.flow_from_directory(
    trainval_dir,
    target_size=(224, 224),
    batch_size=32,
    class_mode='categorical',
    subset='validation',
    shuffle=False
)

# Build model with transfer learning
base_model = MobileNetV2(weights='imagenet', include_top=False, input_shape=(224, 224, 3))
base_model.trainable = False

x = base_model.output
x = GlobalAveragePooling2D()(x)
x = Dropout(0.3)(x)
x = Dense(128, activation='relu')(x)
outputs = Dense(4, activation='softmax')(x)

model = Model(inputs=base_model.input, outputs=outputs)
model.compile(optimizer=Adam(learning_rate=0.0001), 
              loss='categorical_crossentropy', 
              metrics=['accuracy'])

# Train the model
history = model.fit(train_data, epochs=10, validation_data=val_data)
model.save("my_model.keras")
```

### Making Predictions

```python
import tensorflow as tf
import numpy as np
from tensorflow.keras.preprocessing import image
import matplotlib.pyplot as plt

# Load the trained model
model = tf.keras.models.load_model("my_model.keras")

# Class names
class_names = ['Chronic otitis media', 'Earwax plug', 'Myringosclerosis', 'Normal']

def predict_ear_condition(img_path):
    # Load and preprocess image
    img = image.load_img(img_path, target_size=(224, 224))
    img_array = image.img_to_array(img) / 255.0
    img_array = np.expand_dims(img_array, axis=0)
    
    # Make prediction
    predictions = model.predict(img_array)
    predicted_index = np.argmax(predictions, axis=1)[0]
    predicted_class = class_names[predicted_index]
    confidence = predictions[0][predicted_index]
    
    # Display results
    plt.imshow(img)
    plt.axis('off')
    plt.title(f"Predicted: {predicted_class} ({confidence:.2%})")
    plt.show()
    
    return predicted_class, predictions[0]

# Example usage
predicted_class, probabilities = predict_ear_condition("path/to/your/image.jpg")
print(f"Predicted class: {predicted_class}")
print("All probabilities:", probabilities)
```

## 📈 Model Performance

- **Test Accuracy**: 87%
- **Architecture**: MobileNetV2 + Custom Classification Head
- **Training Epochs**: 10
- **Optimizer**: Adam (learning_rate=0.0001)
- **Data Augmentation**: Rotation, zoom, shift, shear, horizontal flip

### Training Results
- Final training accuracy: ~86.6%
- Final validation accuracy: ~81.3%
- Test accuracy: 87%

## 🏗️ Model Architecture

```
Input (224x224x3)
    ↓
MobileNetV2 (frozen, ImageNet weights)
    ↓
GlobalAveragePooling2D
    ↓
Dropout(0.3)
    ↓
Dense(128, ReLU)
    ↓
Dense(4, Softmax)
    ↓
Output (4 classes)
```


## 📝 Future Improvements

- [ ] Implement fine-tuning of base model layers
- [ ] Add more sophisticated data augmentation techniques
- [ ] Experiment with other pre-trained architectures (EfficientNet, ResNet)
- [ ] Implement model interpretability features (GradCAM)
- [ ] Add confidence thresholding for uncertain predictions
- [ ] Expand dataset size for better generalization

