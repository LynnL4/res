import json
import os
import math

def format(dpath):
    with open(os.path.join(dpath, '_annotations.coco.json'), 'r') as f:
        Y_train = json.loads(f.read())
        metadata = {
            "version": 1,
            "type": "bounding-box-labels",
            "boundingBoxes": {

            }
        }

        classes = []
        for label in Y_train['categories']:
            classes.append(label['name'])
        print(classes)

        for image in Y_train['images']:
            metadata['boundingBoxes'][image['file_name']] = []
            for annotation in Y_train['annotations']:
                if annotation['image_id'] == image['id']:
                    metadata['boundingBoxes'][image['file_name']].append({
                        "label": classes[annotation['category_id']],
                        "x": annotation['bbox'][0],
                        "y": annotation['bbox'][1],
                        "width": annotation['bbox'][2],
                        "height": annotation['bbox'][3],
                        
                    })
              
        with open(dpath+'/bounding_boxes.labels', 'w') as fq:
            fq.write(json.dumps(metadata, indent=4))

format('./train')
format('./valid')
format('./test')