import cv2
import mediapipe as mp
import requests
import time

mp_hands = mp.solutions.hands
hands = mp_hands.Hands(
    static_image_mode=False,
    max_num_hands=2,
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5
)

# Open webcam.
cap = cv2.VideoCapture(0)
if not cap.isOpened():
    raise RuntimeError("Could not open webcam.")

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # Flip horizontally for a mirror view.
        frame = cv2.flip(frame, 1)

        # Convert to RGB and run hand detection.
        img_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        img_rgb.flags.writeable = False
        results = hands.process(img_rgb)
        img_rgb.flags.writeable = True

        # Extract landmark coordinates.
        landmark_dict = {}
        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                for i, lm in enumerate(hand_landmarks.landmark):
                    name = mp_hands.HandLandmark(i).name
                    landmark_dict[name] = (lm.x, lm.y, lm.z, lm.visibility)

        # Print out the normalized (x, y, z, visibility) tuples.
        print(landmark_dict)

        try: requests.post("http://localhost:3000/landmarks", json=landmark_dict, timeout=0.01)
        except requests.exceptions.RequestException: pass

        time.sleep(1e-3)
except KeyboardInterrupt: pass
finally:
    cap.release()
    hands.close()