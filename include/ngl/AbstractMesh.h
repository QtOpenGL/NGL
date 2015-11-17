/*
  Copyright (C) 2009 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ABSTRACT_MESH_H__
#define ABSTRACT_MESH_H__
/// @file AbstractMesh.h
/// @brief a series of classes used to define an abstract 3D mesh of Faces, Vertex Normals and TexCords
// must include types.h first for Real and GLEW if required
#include "Types.h"
#include "BBox.h"
#include "RibExport.h"
#include "Texture.h"
#include "NGLassert.h"
#include "Vec4.h"
#include "VertexArrayObject.h"

#include <vector>
#include <string>
#include <cstdint>
// part of the python bindings
#ifdef NO_PYTHON_LIB
  #include <boost/python.hpp>
  #include <boost/format.hpp>
#endif

namespace ngl
{
//----------------------------------------------------------------------------------------------------------------------
/// @class Face  "include/Obj.h"
/// @brief simple class used to encapsulate a single face of an abstract mesh file
/// @todo add the ability to have user installable attribute lists
//----------------------------------------------------------------------------------------------------------------------
class Face
{
public :
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the number of vertices in the face
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int m_numVerts;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief The vertices index
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<uint32_t> m_vert;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief The texture co-ord index
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<uint32_t> m_tex;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the normal index
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<uint32_t> m_norm;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief debug flag to turn face on and off
  //----------------------------------------------------------------------------------------------------------------------
  bool m_textureCoord;
  bool m_normals;
};

//----------------------------------------------------------------------------------------------------------------------
/// @class IndexRef
/// @brief a class to hold the index into vert / norm and tex list for creating the VBO data structure
//----------------------------------------------------------------------------------------------------------------------
class IndexRef
{
public :

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief index into the vert list
  //----------------------------------------------------------------------------------------------------------------------
  uint32_t m_v;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief index into the nornmal list
  //----------------------------------------------------------------------------------------------------------------------
  uint32_t m_n;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief index into the texture list
  //----------------------------------------------------------------------------------------------------------------------
  uint32_t m_t;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief ctor to set the data
  /// @param[in] _v the vert index
  /// @param[in] _n the normal index
  /// @param[in] _t the texture index
  //----------------------------------------------------------------------------------------------------------------------
  IndexRef(uint32_t _v, uint32_t _n, uint32_t _t )  :m_v(_v),m_n(_n),m_t(_t) {;}
};

//----------------------------------------------------------------------------------------------------------------------
/// @class AbstractMesh "include/AbstractMesh.h"
/// @author Jonathan Macey
/// @version 1.0
/// @date 24/09/09
/// Last Revision 16/06/10 added accessors for the mesh elements
/// @brief an abstract base mesh used to build specific meshes such as Obj
//----------------------------------------------------------------------------------------------------------------------
class NGL_DLLEXPORT AbstractMesh
{

public :

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Method to load the file in
  /// @param[in]  &_fname the name of the obj file to load
  //----------------------------------------------------------------------------------------------------------------------
  virtual bool load(const std::string &_fname,bool _calcBB=true)  =0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief default ctor must be called from the child class so our dtor is called
  //----------------------------------------------------------------------------------------------------------------------
  AbstractMesh()  : m_vbo(false),  m_vao(false), m_ext(nullptr) {;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief destructor this will clear out all the vert data and the vbo if created
  //----------------------------------------------------------------------------------------------------------------------
  virtual ~AbstractMesh() ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to draw the bounding box
  //----------------------------------------------------------------------------------------------------------------------
  void drawBBox() const ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief draw method to draw the obj as a VBO. The VBO first needs to be created using the CreateVBO method
  //----------------------------------------------------------------------------------------------------------------------
  void draw() const ;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief load int a texture and set it as the active texture of the Obj
  /// @param[in] &_fname the name of the file to load
  void loadTexture(const std::string& _fname ) ;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief scale the obj by multiplying the actual vertex values by sx,sy and sz
  /// @param[in] _sx the scale value in x
  /// @param[in] _sy the scale value in y
  /// @param[in] _sz the scale value in z
  //----------------------------------------------------------------------------------------------------------------------
  void scale( Real _sx, Real _sy, Real _sz ) ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a method to set the BBox and center
  //----------------------------------------------------------------------------------------------------------------------
  void calcDimensions() ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a method to caluculate the bounding Sphere will set
  /// m_sphereCenter and m_sphereRadius
  //----------------------------------------------------------------------------------------------------------------------
  void calcBoundingSphere() ;

  //----------------------------------------------------------------------------------------------------------------------
  /// method to write out the obj mesh to a renderman sub div
  /// @param[in] _ribFile the instance of the RibExport class
  //----------------------------------------------------------------------------------------------------------------------
  void writeToRibSubdiv( RibExport& _ribFile) const ;

  //----------------------------------------------------------------------------------------------------------------------
  //// @brief create a VAO from the current mesh data
  //----------------------------------------------------------------------------------------------------------------------
  void createVAO() ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the texture id
  /// @returns the texture id
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int getTextureID() const  { return m_textureID; }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief map the VBO vertex data
  /// @returns a pointer to the VBO vertex data
  //----------------------------------------------------------------------------------------------------------------------
  Real *mapVAOVerts() ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief unmap the VBO based
  //----------------------------------------------------------------------------------------------------------------------
  void unMapVAO() ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get a pointer to the indices used to represent the VBO data, this is used in the clip
  /// class when re-ordering the clip data values
  /// @returns the array of indices
  //----------------------------------------------------------------------------------------------------------------------
  const std::vector<IndexRef> & getIndices()  { return m_indices; }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief save the mesh as NCCA Binary VBO format
  /// basically this format is the processed binary vbo mesh data as
  /// as packed by the CreateVBO() method is called.
  //----------------------------------------------------------------------------------------------------------------------
  void saveNCCABinaryMesh( const std::string &_fname ) ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a method to get the current bounding box of the mesh
  /// @returns the bounding box for the loaded mesh;
  //----------------------------------------------------------------------------------------------------------------------

  BBox &getBBox() { return *m_ext;  }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accessor for the vertex data
  /// @returns a std::vector containing the vert data
  //----------------------------------------------------------------------------------------------------------------------
  std::vector <Vec3> getVertexList() {return m_verts;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accessor for the vertex data
  /// @returns a std::vector containing the vert data
  //----------------------------------------------------------------------------------------------------------------------
  Vec3 getVertexAtIndex( uint32_t _i ) const 
  {
    //NGL_ASSERT(_i>0 && _i<m_nVerts);
    return m_verts[_i];
  }

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accessor for the normals data
  /// @returns a std::vector containing the normal data
  //----------------------------------------------------------------------------------------------------------------------
  std::vector <Vec3> getNormalList() {return m_norm;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accessor for the texture co-ordinates data
  /// @returns a std::vector containing the texture cord data
  //----------------------------------------------------------------------------------------------------------------------
  std::vector <Vec3> getTextureCordList() {return m_tex;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accessor for the Face data
  /// @returns a std::vector containing the face data
  //----------------------------------------------------------------------------------------------------------------------
  std::vector <Face> getFaceList() {return m_face;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accessor to get the number of vertices in the object
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int getNumVerts() const  {return m_nVerts;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accessor to get the number of normals in the object
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int getNumNormals()const  {return m_nNorm;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accessor to get the number of texture co-ordinates in the object
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int getNumTexCords()const  {return m_nTex;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accessor to get the number of faces in the object
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int getNumFaces()const  {return m_nFaces;}
  unsigned int getMeshSize()const  {return m_meshSize;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accesor to get the bounding sphere center
  //----------------------------------------------------------------------------------------------------------------------
  Vec3 getSphereCenter() const  {return m_sphereCenter;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accesor to get the bounding sphere center
  //----------------------------------------------------------------------------------------------------------------------
  Real getSphereRadius() const  {return m_sphereRadius;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accesor to get the center
  //----------------------------------------------------------------------------------------------------------------------
  Vec3 getCenter() const  {return m_center;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief check to see if obj is triangulated as we only support tri or quad objs at the moment
  /// @returns true or false
  //----------------------------------------------------------------------------------------------------------------------
  bool isTriangular() ;

protected :
  friend class NCCAPointBake;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief The number of vertices in the object
  unsigned int m_nVerts;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief The number of normals in the object
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int m_nNorm;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the number of texture co-ordinates in the object
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int m_nTex;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the number of faces in the object
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int m_nFaces;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Pointer to the Vertex list
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<Vec3> m_verts;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Pointer to the Normal List
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<Vec3> m_norm;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Pointer to the Texture co-ord list (note that only x and y are used)
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<Vec3> m_tex;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Pointer to the Face list
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<Face> m_face;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Center of the object
  //----------------------------------------------------------------------------------------------------------------------
  Vec3 m_center;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a vector of indices used to pass the index into the Data arrays to the VBO
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<IndexRef> m_indices;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a vectr of indices without duplicates which are actually passed to the VBO when creating
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<GLuint> m_outIndices;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the size of the index array
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int m_indexSize;
  unsigned int m_meshSize;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief buffers for the VBO in order Vert, Tex, Norm
  //----------------------------------------------------------------------------------------------------------------------
  GLuint m_vboBuffers;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief id for our vertexArray object
  //----------------------------------------------------------------------------------------------------------------------
  VertexArrayObject *m_vaoMesh;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief flag to indicate if a VBO has been created
  //----------------------------------------------------------------------------------------------------------------------
  bool m_vbo;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief flag to indicate if a VBO has been created
  //----------------------------------------------------------------------------------------------------------------------
  bool m_vao;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief flag to indicate if the VBO vertex data has been mapped
  //----------------------------------------------------------------------------------------------------------------------
  bool m_vboMapped;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief flag to indicate if texture assigned
  //----------------------------------------------------------------------------------------------------------------------
  bool m_texture;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief The openGL id  of the texture for the texture generation
  //----------------------------------------------------------------------------------------------------------------------
  GLuint m_textureID;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief The Maximum X value in the obj file used to calculate the extents bbox
  //----------------------------------------------------------------------------------------------------------------------
  Real m_maxX;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief The Min X value in the obj file used to calculate the extents bbox
  //----------------------------------------------------------------------------------------------------------------------
  Real m_minX;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  The Max Y value in the obj file used to calculate the extents bbox
  //----------------------------------------------------------------------------------------------------------------------
  Real m_maxY;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  The Min Y value in the obj file used to calculate the extents bbox
  //----------------------------------------------------------------------------------------------------------------------
  Real m_minY;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  The Max Z value in the obj file used to calculate the extents bbox
  //----------------------------------------------------------------------------------------------------------------------
  Real m_maxZ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  The Min Z value in the obj file used to calculate the extents bbox
  //----------------------------------------------------------------------------------------------------------------------
  Real m_minZ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief   Create a bounding box of the object to store it's extents
  //----------------------------------------------------------------------------------------------------------------------
  BBox *m_ext;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  determines if the data is Packed as either TRI or QUAD
  //----------------------------------------------------------------------------------------------------------------------
  GLuint m_dataPackType;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  the size of the buffer pack we use this for the saving of bin vbo
  /// but it actually holds the size of the data (3 for just verts 5 verts norms etc)
  //----------------------------------------------------------------------------------------------------------------------
  int  m_bufferPackSize;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  which type of VBO are we going to draw
  //----------------------------------------------------------------------------------------------------------------------
  GLenum m_vboDrawType;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  flag to indicate if anything loaded for dtor
  //----------------------------------------------------------------------------------------------------------------------
  bool m_loaded;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  the center of the bounding sphere
  //----------------------------------------------------------------------------------------------------------------------
  Vec3 m_sphereCenter;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  the radius of the bounding sphere
  //----------------------------------------------------------------------------------------------------------------------
  Real m_sphereRadius;
#ifdef NO_PYTHON_LIB
  public :
    boost::python::list getVertexListPY();
    boost::python::list getNormalListPY();
    boost::python::list getTextureCordListPY();

#endif

};

} // end of namespace ngl


#endif // end of the class
