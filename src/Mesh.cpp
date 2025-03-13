#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices,
           std::string name) {
  mVertices = vertices;
  mIndices = indices;
  mName = name;

  mVAO.Bind();
  VBO mVBO(vertices);
  EBO mEBO(indices);
  mVAO.LinkAttrib(mVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
  mVAO.LinkAttrib(mVBO, 1, 3, GL_FLOAT, sizeof(Vertex),
                  (void *)(3 * sizeof(float)));
  mVAO.LinkAttrib(mVBO, 2, 3, GL_FLOAT, sizeof(Vertex),
                  (void *)(6 * sizeof(float)));
  mVAO.Unbind();
  mVBO.Unbind();
  mEBO.Unbind();
}

void Mesh::Draw(Shader &shader, Camera &camera, GLuint mode) const {
  shader.Activate();
  mVAO.Bind();

  shader.SetMat4("model", mModel);

  shader.SetVec3("camPos", camera.GetPosition());
  camera.Matrix(shader, "camMatrix");

  glDrawElements(mode, mIndices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::SetTransform(const glm::mat4 &mat) { mModel = mat; }

Mesh Mesh::CreatePlane(float size, const glm::vec3 &color) {
  glm::vec3 normal = glm::vec3(0, 0, 1);
  std::vector<Vertex> vertices = {
      {glm::vec3(size / 2, size / 2, 0), color, normal},
      {glm::vec3(-size / 2, size / 2, 0), color, normal},
      {glm::vec3(size / 2, -size / 2, 0), color, normal},
      {glm::vec3(-size / 2, -size / 2, 0), color, normal},
  };
  std::vector<unsigned int> indices = {0, 1, 2, 1, 2, 3};
  return Mesh(vertices, indices, "Plane");
}

Mesh Mesh::CreateCube(float size, const glm::vec3 &color) {
  // glm::vec3 color = glm::vec3(0.8f);
  float d = size / 2.0f;

  // Define positions for the eight cube vertices
  glm::vec3 ppp = glm::vec3(d, d, d);
  glm::vec3 npp = glm::vec3(-d, d, d);
  glm::vec3 pnp = glm::vec3(d, -d, d);
  glm::vec3 nnp = glm::vec3(-d, -d, d);
  glm::vec3 ppn = glm::vec3(d, d, -d);
  glm::vec3 npn = glm::vec3(-d, d, -d);
  glm::vec3 pnn = glm::vec3(d, -d, -d);
  glm::vec3 nnn = glm::vec3(-d, -d, -d);

  // Define reusable normals for each face direction
  glm::vec3 normals[] = {
      {0.0f, 0.0f, 1.0f},  // Front face
      {0.0f, 0.0f, -1.0f}, // Back face
      {-1.0f, 0.0f, 0.0f}, // Left face
      {1.0f, 0.0f, 0.0f},  // Right face
      {0.0f, 1.0f, 0.0f},  // Top face
      {0.0f, -1.0f, 0.0f}  // Bottom face
  };

  // Define reusable texture coordinates for each vertex in a face
  glm::vec2 texCoords[] = {
      {1.0f, 1.0f}, // Top Right
      {1.0f, 0.0f}, // Bottom Right
      {0.0f, 0.0f}, // Bottom Left
      {0.0f, 1.0f}  // Top Left
  };

  // Define vertices for the cube, using normals and texCoords
  std::vector<Vertex> cubeVertices = {
      // Front face
      {ppp, color, normals[0]}, // Top Right
      {pnp, color, normals[0]}, // Bottom Right
      {nnp, color, normals[0]}, // Bottom Left
      {npp, color, normals[0]}, // Top Left

      // Back face
      {ppn, color, normals[1]}, // Top Right
      {pnn, color, normals[1]}, // Bottom Right
      {nnn, color, normals[1]}, // Bottom Left
      {npn, color, normals[1]}, // Top Left

      // Left face
      {npp, color, normals[2]}, // Top Right
      {nnp, color, normals[2]}, // Bottom Right
      {nnn, color, normals[2]}, // Bottom Left
      {npn, color, normals[2]}, // Top Left

      // Right face
      {ppp, color, normals[3]}, // Top Right
      {pnp, color, normals[3]}, // Bottom Right
      {pnn, color, normals[3]}, // Bottom Left
      {ppn, color, normals[3]}, // Top Left

      // Top face
      {ppp, color, normals[4]}, // Top Right
      {npp, color, normals[4]}, // Top Left
      {npn, color, normals[4]}, // Bottom Left
      {ppn, color, normals[4]}, // Bottom Right

      // Bottom face
      {pnp, color, normals[5]}, // Top Right
      {nnp, color, normals[5]}, // Top Left
      {nnn, color, normals[5]}, // Bottom Left
      {pnn, color, normals[5]}  // Bottom Right
  };

  // Define indices for the cube (same as before)
  std::vector<unsigned int> cubeIndices = {// Front face
                                           0, 1, 3, 1, 2, 3,
                                           // Back face
                                           4, 5, 7, 5, 6, 7,
                                           // Left face
                                           8, 9, 11, 9, 10, 11,
                                           // Right face
                                           12, 13, 15, 13, 14, 15,
                                           // Top face
                                           16, 17, 19, 17, 18, 19,
                                           // Bottom face
                                           20, 21, 23, 21, 22, 23};

  // Initialize the Mesh with cube data
  return Mesh(cubeVertices, cubeIndices, "Cube");
}

Mesh Mesh::CreateSphere(float radius, const glm::vec3 &color) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  int sectorCount = 16;
  int stackCount = sectorCount;
  // glm::vec3 color = glm::vec3(0.8f);

  float sectorStep = 2 * M_PI / sectorCount;
  float stackStep = M_PI / stackCount;
  float sectorAngle, stackAngle;

  // Generate vertices
  for (unsigned int i = 0; i <= stackCount; ++i) {
    stackAngle = M_PI / 2 - i * stackStep; // from pi/2 to -pi/2
    float xy = radius * cosf(stackAngle);  // r * cos(u)
    float z = radius * sinf(stackAngle);   // r * sin(u)

    for (unsigned int j = 0; j <= sectorCount; ++j) {
      sectorAngle = j * sectorStep; // from 0 to 2pi

      // Vertex position
      float x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
      float y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
      glm::vec3 position(x, y, z);

      // Vertex normal
      glm::vec3 normal =
          glm::normalize(position); // Normalized position as normal

      vertices.push_back({position, color, normal});
    }
  }

  // Generate indices
  for (unsigned int i = 0; i < stackCount; ++i) {
    unsigned int k1 = i * (sectorCount + 1); // beginning of current stack
    unsigned int k2 = k1 + sectorCount + 1;  // beginning of next stack

    for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
      // 2 triangles per sector excluding the first and last stacks
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      if (i != (stackCount - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }
  return Mesh(vertices, indices, "Sphere");
}

Mesh Mesh::CreateCamera() {
  glm::vec3 color = glm::vec3(1,0,1);
  float x = 5.5f;
  float y = 5.5f;
  float z = 2.25f;
  std::vector<Vertex> vertices = {{glm::vec3(0, 0, 0), color, color},
                                  {glm::vec3(x, -y, z), color, color},
                                  {glm::vec3(x, y, z), color, color},
                                  {glm::vec3(x, -y, -z), color, color},
                                  {glm::vec3(x, y, -z), color, color}};
  std::vector<unsigned int> indices = {0, 1, 3, 0, 1, 2, 0, 2, 4, 0, 3, 4};
  return Mesh(vertices, indices);
}

Mesh Mesh::CreateFrustum(const Camera& cam, float nearPlane, float farPlane, const glm::vec3& color) {
    // Compute the width and height of the near and far planes
  glm::mat3 K = cam.GetParameters().Intrinsic;
    float halfW_near = nearPlane * (K[2][0] / K[0][0]);
    float halfH_near = nearPlane * (K[2][1] / K[1][1]);
    float halfW_far = farPlane * (K[2][0] / K[0][0]);
    float halfH_far = farPlane * (K[2][1] / K[1][1]);

    // Frustum corners in camera space
    std::vector<glm::vec4> frustumPoints = {
        {-halfW_near, -halfH_near, nearPlane, 1.0f}, // Near bottom left
        { halfW_near, -halfH_near, nearPlane, 1.0f}, // Near bottom right
        {-halfW_near,  halfH_near, nearPlane, 1.0f}, // Near top left
        { halfW_near,  halfH_near, nearPlane, 1.0f}, // Near top right
        {-halfW_far,  -halfH_far,  farPlane, 1.0f},  // Far bottom left
        { halfW_far,  -halfH_far,  farPlane, 1.0f},  // Far bottom right
        {-halfW_far,   halfH_far,  farPlane, 1.0f},  // Far top left
        { halfW_far,   halfH_far,  farPlane, 1.0f}   // Far top right
    };

    // Transform frustum points from camera space to world space
    glm::mat4 invRT = glm::inverse(cam.GetMatrix());
    std::vector<Vertex> frustumVertices;
    for (auto& pt : frustumPoints) {
        glm::vec4 worldPt = invRT * pt;
        frustumVertices.push_back({glm::vec3(worldPt), color, glm::vec3(0.0f)});
        // frustumVertices.push_back({pt, color, glm::vec3(0.0f)});
    }

    // Define indices for frustum faces
    std::vector<unsigned int> frustumIndices = {
        // Near plane
        0, 1, 3, 0, 3, 2,
        // Far plane
        4, 5, 7, 4, 7, 6,
        // Left plane
        0, 2, 6, 0, 6, 4,
        // Right plane
        1, 3, 7, 1, 7, 5,
        // Top plane
        2, 3, 7, 2, 7, 6,
        // Bottom plane
        0, 1, 5, 0, 5, 4
    };

    return Mesh(frustumVertices, frustumIndices, "Frustum");
}
