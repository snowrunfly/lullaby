/*
Copyright 2017 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS-IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "lullaby/tools/common/fbx_utils.h"

namespace lull {
namespace tool {

static FbxAxisSystem AxisSystemToFbxAxisSystem(AxisSystem system) {
  int up = FbxAxisSystem::eXAxis;
  int front = FbxAxisSystem::eParityEven;
  int coord = FbxAxisSystem::eRightHanded;
  switch (system) {
    case AxisSystem_Unspecified:
      break;
    case AxisSystem_XUp_YFront_ZLeft:
      up = FbxAxisSystem::eXAxis;
      front = FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_XUp_YFront_ZRight:
      up = FbxAxisSystem::eXAxis;
      front = FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_XUp_YBack_ZLeft:
      up = FbxAxisSystem::eXAxis;
      front = FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_XUp_YBack_ZRight:
      up = FbxAxisSystem::eXAxis;
      front = FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_XUp_ZFront_YLeft:
      up = FbxAxisSystem::eXAxis;
      front = -FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_XUp_ZFront_YRight:
      up = FbxAxisSystem::eXAxis;
      front = -FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_XUp_ZBack_YLeft:
      up = FbxAxisSystem::eXAxis;
      front = -FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_XUp_ZBack_YRight:
      up = FbxAxisSystem::eXAxis;
      front = -FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_YUp_XFront_ZLeft:
      up = FbxAxisSystem::eYAxis;
      front = FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_YUp_XFront_ZRight:
      up = FbxAxisSystem::eYAxis;
      front = FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_YUp_XBack_ZLeft:
      up = FbxAxisSystem::eYAxis;
      front = FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_YUp_XBack_ZRight:
      up = FbxAxisSystem::eYAxis;
      front = FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_YUp_ZFront_XLeft:
      up = FbxAxisSystem::eYAxis;
      front = -FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_YUp_ZFront_XRight:
      up = FbxAxisSystem::eYAxis;
      front = -FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_YUp_ZBack_XLeft:
      up = FbxAxisSystem::eYAxis;
      front = -FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_YUp_ZBack_XRight:
      up = FbxAxisSystem::eYAxis;
      front = -FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_ZUp_XFront_YLeft:
      up = FbxAxisSystem::eZAxis;
      front = FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_ZUp_XFront_YRight:
      up = FbxAxisSystem::eZAxis;
      front = FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_ZUp_XBack_YLeft:
      up = FbxAxisSystem::eZAxis;
      front = FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_ZUp_XBack_YRight:
      up = FbxAxisSystem::eZAxis;
      front = FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_ZUp_YFront_XLeft:
      up = FbxAxisSystem::eZAxis;
      front = -FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_ZUp_YFront_XRight:
      up = FbxAxisSystem::eZAxis;
      front = -FbxAxisSystem::eParityEven;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    case AxisSystem_ZUp_YBack_XLeft:
      up = FbxAxisSystem::eZAxis;
      front = -FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eRightHanded;
      break;
    case AxisSystem_ZUp_YBack_XRight:
      up = FbxAxisSystem::eZAxis;
      front = -FbxAxisSystem::eParityOdd;
      coord = FbxAxisSystem::eLeftHanded;
      break;
    default:
      break;
  }
  return FbxAxisSystem(static_cast<FbxAxisSystem::EUpVector>(up),
                       static_cast<FbxAxisSystem::EFrontVector>(front),
                       static_cast<FbxAxisSystem::ECoordSystem>(coord));
}

FbxBaseImporter::FbxBaseImporter() {
  manager_ = FbxManager::Create();
  if (manager_ == nullptr) {
    LOG(ERROR) << "Unable to create FBX manager";
    return;
  }

  // Initialize with standard IO settings.
  FbxIOSettings* ios = FbxIOSettings::Create(manager_, IOSROOT);
  manager_->SetIOSettings(ios);

  // Create an FBX scene. This holds the objects imported from files.
  scene_ = FbxScene::Create(manager_, "Scene");
  if (scene_ == nullptr) {
    LOG(ERROR) << "Unable to create FBX scene.";
    return;
  }
}

FbxBaseImporter::~FbxBaseImporter() {
  if (manager_ != nullptr) {
    manager_->Destroy();
  }
}

bool FbxBaseImporter::LoadScene(const std::string& filename) {
  if (manager_ == nullptr || scene_ == nullptr) {
    LOG(ERROR) << "FBX Manager and Scene not created.";
    return false;
  }

  FbxImporter* importer = FbxImporter::Create(manager_, "");
  const bool init_success =
      importer->Initialize(filename.c_str(), -1, manager_->GetIOSettings());

  if (!init_success) {
    LOG(ERROR) << "Failed loading: " << importer->GetStatus().GetErrorString();
    LOG(ERROR) << "FBX SDK Version: " << GetSdkVersion();
    LOG(ERROR) << "FBX File Version: " << GetFileVersion(importer);
    return false;
  }

  const bool import_success = importer->Import(scene_);
  importer->Destroy();

  if (!import_success) {
    LOG(ERROR) << "Failed import: " << importer->GetStatus().GetErrorString();
    return false;
  }

  return true;
}

FbxNode* FbxBaseImporter::GetRootNode() { return scene_->GetRootNode(); }

std::string FbxBaseImporter::GetSdkVersion() const {
  int major = 0;
  int minor = 0;
  int revision = 0;
  FbxManager::GetFileFormatVersion(major, minor, revision);
  std::stringstream version;
  version << major << "." << minor << "." << revision;
  return version.str();
}

std::string FbxBaseImporter::GetFileVersion(FbxImporter* importer) const {
  int major = 0;
  int minor = 0;
  int revision = 0;
  importer->GetFileVersion(major, minor, revision);
  std::stringstream version;
  version << major << "." << minor << "." << revision;
  return version.str();
}

std::vector<FbxBaseImporter::BoneInfo> FbxBaseImporter::BuildBoneList() {
  FbxNode* root_node = scene_->GetRootNode();

  // Skip the root node so it's not included in the bone list.
  const int child_count = root_node->GetChildCount();

  // First determine which nodes are to be treated as bones.
  std::set<FbxNode*> valid_nodes;
  for (int child_index = 0; child_index != child_count; ++child_index) {
    FbxNode* child_node = root_node->GetChild(child_index);
    MarkBoneNodesRecursive(child_node, &valid_nodes);
  }

  // Go through the nodes again, adding the bones in order to the bone list.
  std::vector<BoneInfo> bones;
  for (int child_index = 0; child_index != child_count; ++child_index) {
    FbxNode* child_node = root_node->GetChild(child_index);
    BoneInfo info = {child_node, kInvalidBoneIndex};
    BuildBonesRecursive(info, valid_nodes, &bones);
  }

  return bones;
}

bool FbxBaseImporter::MarkBoneNodesRecursive(FbxNode* node,
                                             std::set<FbxNode*>* valid_nodes) {
  bool valid_bone = false;
  if (node) {
    // We need a bone for this node if it has a skeleton attribute or a mesh.
    valid_bone = (node->GetSkeleton() || node->GetMesh());

    // We also need a bone for this node if it has any valid child bones.
    const int child_count = node->GetChildCount();
    for (int child_index = 0; child_index != child_count; ++child_index) {
      FbxNode* child_node = node->GetChild(child_index);
      if (MarkBoneNodesRecursive(child_node, valid_nodes)) {
        valid_bone = true;
      }
    }
  }

  // Flag the node as a bone.
  if (valid_bone) {
    valid_nodes->emplace(node);
  }
  return valid_bone;
}

void FbxBaseImporter::BuildBonesRecursive(const BoneInfo& bone,
                                          const std::set<FbxNode*>& valid_nodes,
                                          std::vector<BoneInfo>* bones) {
  if (valid_nodes.count(bone.node) == 0) {
    return;
  }

  const int bone_index = static_cast<int>(bones->size());
  bones->emplace_back(bone);

  const int child_count = bone.node->GetChildCount();
  for (int child_index = 0; child_index != child_count; ++child_index) {
    FbxNode* child_node = bone.node->GetChild(child_index);
    BoneInfo child_bone = {child_node, bone_index};
    BuildBonesRecursive(child_bone, valid_nodes, bones);
  }
}

void FbxBaseImporter::ConvertFbxScale(float distance_unit) {
  if (distance_unit <= 0.0f) {
    return;
  }
  const FbxSystemUnit import_unit = scene_->GetGlobalSettings().GetSystemUnit();
  const FbxSystemUnit export_unit(distance_unit);
  if (import_unit != export_unit) {
    export_unit.ConvertScene(scene_);
  }
}

void FbxBaseImporter::ConvertFbxAxes(AxisSystem axis_system) {
  if (axis_system == AxisSystem_Unspecified) {
    return;
  }

  const FbxAxisSystem import_axes = scene_->GetGlobalSettings().GetAxisSystem();
  const FbxAxisSystem export_axes = AxisSystemToFbxAxisSystem(axis_system);
  if (import_axes != export_axes) {
    export_axes.ConvertScene(scene_);
  }

  // The FBX SDK has a bug. After an axis conversion, the prerotation is not
  // propagated to the PreRotation property. We propagate the values manually.
  // Note that we only propagate to the children of the root, since those are
  // the only nodes affected by axis conversion.
  FbxNode* root = scene_->GetRootNode();
  for (int i = 0; i < root->GetChildCount(); i++) {
    FbxNode* node = root->GetChild(i);
    node->PreRotation.Set(node->GetPreRotation(FbxNode::eSourcePivot));
  }
}

void FbxBaseImporter::ConvertGeometry(bool recenter) {
  FbxGeometryConverter geo_converter(manager_);
  if (recenter) {
    geo_converter.RecenterSceneToWorldCenter(scene_, 0.0);
  }
  geo_converter.SplitMeshesPerMaterial(scene_, true);
  geo_converter.Triangulate(scene_, true);
  ConvertGeometryRecursive(scene_->GetRootNode());
}

void FbxBaseImporter::ConvertGeometryRecursive(FbxNode* node) {
  if (node == nullptr) {
    return;
  }

  for (int i = 0; i < node->GetNodeAttributeCount(); ++i) {
    FbxNodeAttribute* attr = node->GetNodeAttributeByIndex(i);
    if (attr == nullptr) {
      continue;
    } else if (attr->GetAttributeType() != FbxNodeAttribute::eMesh) {
      continue;
    }
    FbxMesh* mesh = static_cast<FbxMesh*>(attr);
    mesh->GenerateNormals();
    mesh->GenerateTangentsData(0);
  }

  // Recursively traverse each node in the scene
  for (int i = 0; i < node->GetChildCount(); i++) {
    ConvertGeometryRecursive(node->GetChild(i));
  }
}

}  // namespace tool
}  // namespace lull
