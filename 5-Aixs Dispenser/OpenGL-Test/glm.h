#ifndef _GLM_H
#define _GLM_H

#pragma once
/*
glm.h
Nate Robins, 1997, 2000
nate@pobox.com, http://www.pobox.com/~nate
Wavefront OBJ model file format reader/writer/manipulator.
Includes routines for generating smooth normals with
preservation of edges, welding redundant vertices & texture
coordinate generation (spheremap and planar projections) + more.
Modified by Ramgopal R to accomodate point rendering.
glm庫函數解釋：by Yvan Liu
http://blog.csdn.net/LotusOne/article/details/5082515
*/
#include <stdlib.h>
#include <gl\glut.h>

#define GLM_POINTS   (1 << 0)       /* render with only vertices */
#define GLM_FLAT     (1 << 1)       /* render with facet normals */
#define GLM_SMOOTH   (1 << 2)       /* render with vertex normals */
#define GLM_TEXTURE  (1 << 3)       /* render with texture coords */
#define GLM_COLOR    (1 << 4)       /* render with colors */
#define GLM_MATERIAL (1 << 5)       /* render with materials */
#ifdef __cplusplus
//告訴編譯器將extern "C" 後面的代碼當做C代碼來處理
extern "C" {
#endif
	/* GLMmaterial: Structure that defines a material in a model.
	*/
	//typedef為C語言中的關鍵字，GLMmaterial為_GLMmaterial的別名
	//typedef是类型定义的意思。typedef struct 是为了使用这个结构体方便。
	/*定義模型中的一種材質*/
	typedef struct _GLMmaterial
	{
		char* name;                   /* name of material 材質名稱*/
		GLfloat diffuse[4];           /* diffuse component 漫反射分量*/
		GLfloat ambient[4];           /* ambient component 環境光分量*/
		GLfloat specular[4];          /* specular component 鏡面光分量*/
		GLfloat emmissive[4];         /* emmissive component 散射分量*/
		GLfloat shininess;            /* specular exponent */
		GLfloat illum;
		char textureImageName[128];
		int textureID;
	} GLMmaterial;

	/* GLMtriangle: Structure that defines a triangle in a model.
	定義模型中的一個三角形
	*/
	typedef struct _GLMtriangle {
		GLuint vindices[3];           /* array of triangle vertex indices 三角形頂點*/
		GLuint nindices[3];           /* array of triangle normal indices 法向量*/
		GLuint tindices[3];           /* array of triangle texcoord indices 紋理坐標*/
		GLuint findex;                /* index of triangle facet normal 面法向量索引*/
	} GLMtriangle;

	/* GLMgroup: Structure that defines a group in a model.
	定義模型中的一個組
	*/
	typedef struct _GLMgroup {
		char*             name;           /* name of this group 組的名字*/
		GLuint            numtriangles;   /* number of triangles in this group 組中的三角形的個數*/
		GLuint*           triangles;      /* array of triangle indices 三角形的頂點指針*/
		GLuint            material;       /* index to material for group 組使用的材質索引*/
		struct _GLMgroup* next;           /* pointer to next group in model 模型中下一個組的指針*/
	} GLMgroup;

	/* GLMmodel: Structure that defines a model.
	定義一個模型
	*/
	typedef struct _GLMmodel {
		char*    pathname;            /* path to this model 模型的路徑*/
		char*    mtllibname;          /* name of the material library 材質庫的名字*/
		GLuint   numvertices;         /* number of vertices in model 模型中頂點的數目*/
		GLfloat* vertices;            /* array of vertices  頂點數據*/
		GLfloat* colors;
		GLuint   numnormals;          /* number of normals in model 模型中法向量數目*/
		GLfloat* normals;             /* array of normals 法向量數據*/
		GLuint   numtexcoords;        /* number of texcoords in model 模型中紋理坐標的數目*/
		GLfloat* texcoords;           /* array of texture coordinates 紋理坐標數據*/
		GLuint   numfacetnorms;       /* number of facetnorms in model 模型中面法向量的數目*/
		GLfloat* facetnorms;          /* array of facetnorms 面法向量數據*/
		GLuint       numtriangles;    /* number of triangles in model 模型中三角形的數目*/
		GLMtriangle* triangles;       /* array of triangles 三角形數據*/
		GLuint       nummaterials;    /* number of materials in model 模型中材質的數據*/
		GLMmaterial* materials;       /* array of materials 材質數據*/
		GLuint       numgroups;       /* number of groups in model 模型中組的數目*/
		GLMgroup*    groups;          /* linked list of groups 組的數據*/
		GLfloat position[3];          /* position of the model 模型所在的位置*/
	} GLMmodel;

	/* glmUnitize: "unitize" a model by translating it to the origin and
	* scaling it to fit in a unit cube around the origin.  Returns the
	* scale factor used.
	* 統一化：单位化模型 model 并返回缩放比例因子
	单位化就是把模型通过平移和缩放变换限制到3维坐标系中点为中心的一个单位正方体区域内
	* http://blog.csdn.net/miranda2002/article/details/1767090
	* model - properly initialized GLMmodel structure
	*/
	GLfloat
		glmUnitize(GLMmodel* model);

	/* glmDimensions: Calculates the dimensions (width, height, depth) of
	* a model.
	* 计算模型的宽，高，深尺寸，结果保存在 dimensions 所指的3元素数组中
	* model      - initialized GLMmodel structure
	* dimensions - array of 3 GLfloats (GLfloat dimensions[3])
	*/
	GLvoid
		glmDimensions(GLMmodel* model, GLfloat* dimensions);

	/* glmScale: Scales a model by a given amount.
	* 按比例参数缩放模型，参数大于1放大，大于0小于1缩小，小于0反射，等于0缩小到0
	* model - properly initialized GLMmodel structure
	* scale - scale factor (0.5 = half as large, 2.0 = twice as large)
	*/
	GLvoid
		glmScale(GLMmodel* model, GLfloat scale);

	/* glmReverseWinding: Reverse the polygon winding for all polygons in
	* this model.  Default winding is counter-clockwise.  Also changes
	* the direction of the normals.
	* 反转模型的多边形顶点顺序，同时反转法向量，默认多边形顶点顺序是逆时针的。
	* model - properly initialized GLMmodel structure
	*/
	GLvoid
		glmReverseWinding(GLMmodel* model);

	/* glmFacetNormals: Generates facet normals for a model (by taking the
	* cross product of the two vectors derived from the sides of each
	* triangle).  Assumes a counter-clockwise winding.
	* 计算模型面的法向 （假定多边形顶点顺序为逆时针）
	* model - initialized GLMmodel structure
	*/
	GLvoid
		glmFacetNormals(GLMmodel* model);

	/* glmVertexNormals: Generates smooth vertex normals for a model.
	* First builds a list of all the triangles each vertex is in.  Then
	* loops through each vertex in the the list averaging all the facet
	* normals of the triangles each vertex is in.  Finally, sets the
	* normal index in the triangle for the vertex to the generated smooth
	* normal.  If the dot product of a facet normal and the facet normal
	* associated with the first triangle in the list of triangles the
	* current vertex is in is greater than the cosine of the angle
	* parameter to the function, that facet normal is not added into the
	* average normal calculation and the corresponding vertex is given
	* the facet normal.  This tends to preserve hard edges.  The angle to
	* use depends on the model, but 90 degrees is usually a good start.
	* 计算模型的平滑顶点法向， angle参数为平滑交叉的最大角度（角度制）
	* model - initialized GLMmodel structure
	* angle - maximum angle (in degrees) to smooth across
	*/
	GLvoid
		glmVertexNormals(GLMmodel* model, GLfloat angle);

	/* glmLinearTexture: Generates texture coordinates according to a
	* linear projection of the texture map.  It generates these by
	* linearly mapping the vertices onto a square.
	* 按线性投影产生纹理坐标，它把顶点线性映射到矩形上
	* model - pointer to initialized GLMmodel structure
	*/
	GLvoid
		glmLinearTexture(GLMmodel* model);

	/* glmSpheremapTexture: Generates texture coordinates according to a
	* spherical projection of the texture map.  Sometimes referred to as
	* spheremap, or reflection map texture coordinates.  It generates
	* these by using the normal to calculate where that vertex would map
	* onto a sphere.  Since it is impossible to map something flat
	* perfectly onto something spherical, there is distortion at the
	* poles.  This particular implementation causes the poles along the X
	* axis to be distorted.
	* 按球形映射产生纹理坐标
	* model - pointer to initialized GLMmodel structure
	*/
	GLvoid
		glmSpheremapTexture(GLMmodel* model);

	/* glmDelete: Deletes a GLMmodel structure.
	* 从内存中释放模型
	* model - initialized GLMmodel structure
	*/
	GLvoid
		glmDelete(GLMmodel* model);

	/* glmReadOBJ: Reads a model description from a Wavefront .OBJ file.
	* Returns a pointer to the created object which should be free'd with
	* glmDelete().
	* 从Wavefront公司标准的.OBJ文件中读取模型
	* filename - name of the file containing the Wavefront .OBJ format data.
	*/
	GLMmodel*
		glmReadOBJ(const char*filename);

	/* glmWriteOBJ: Writes a model description in Wavefront .OBJ format to
	* a file.
	*
	* model    - initialized GLMmodel structure
	* filename - name of the file to write the Wavefront .OBJ format data to
	* mode     - a bitwise or of values describing what is written to the file
	*            GLM_NONE    -  write only vertices
	*            GLM_FLAT    -  write facet normals
	*            GLM_SMOOTH  -  write vertex normals
	*            GLM_TEXTURE -  write texture coords
	*            GLM_FLAT and GLM_SMOOTH should not both be specified.
	* 将模型按Wavefront .OBJ文件格式写入文件，文件名由filename参数指定
	* mode 指定写入方式，此参数为取或（“|”）的位联合：
	*            GLM_NONE    - 只按顶点处理
	*			 GLM_FLAT    - 按面计算法向
	*			 GLM_SMOOTH - 按顶点计算法向
	*			 GLM_TEXTURE - 包含纹理座标
	*			 GLM_COLOR    - 只包含颜色信息（纯色材质）
	*			 GLM_MATERIAL - 包含材质信息
	*            其中GLM_FLAT和GLM_SMOOTH不能同时指定
	*			 GLM_COLOR和GLM_MATERIAL也不能同时指定
	*/
	GLvoid
		glmWriteOBJ(GLMmodel* model, const char*filename, GLuint mode);

	/* glmDraw: Renders the model to the current OpenGL context using the
	* mode specified.
	*
	* model    - initialized GLMmodel structure
	* mode     - a bitwise OR of values describing what is to be rendered.
	*            GLM_NONE    -  render with only vertices
	*            GLM_FLAT    -  render with facet normals
	*            GLM_SMOOTH  -  render with vertex normals
	*            GLM_TEXTURE -  render with texture coords
	*            GLM_FLAT and GLM_SMOOTH should not both be specified.
	* 按mode指定模式使用当前的OPENGL绘制上下文（context）绘制模型
	* mode参数同glmReadOBJ中的mode参数
	*/
	GLvoid
		glmDraw(GLMmodel* model, GLuint mode);

	/* glmList: Generates and returns a display list for the model using
	* the mode specified.
	*
	* model    - initialized GLMmodel structure
	* mode     - a bitwise OR of values describing what is to be rendered.
	*            GLM_NONE    -  render with only vertices
	*            GLM_FLAT    -  render with facet normals
	*            GLM_SMOOTH  -  render with vertex normals
	*            GLM_TEXTURE -  render with texture coords
	*            GLM_FLAT and GLM_SMOOTH should not both be specified.
	* 由模型生成OPENGL显示列表并返回显示列表索引号
	* 其中mode参数同glmWriteOBJ和glmDraw中的mode参数
	*/
	GLuint
		glmList(GLMmodel* model, GLuint mode);

	/* glmWeld: eliminate (weld) vectors that are within an epsilon of
	* each other.
	*
	* model      - initialized GLMmodel structure
	* epsilon    - maximum difference between vertices
	*              ( 0.00001 is a good start for a unitized model)
	* 合并模型中差别很小的顶点，epsilon参数指定要合并顶点间的最大差距
	* 推荐epsilon参数0.00001为焊接模型的起点
	*/
	GLvoid
		glmWeld(GLMmodel* model, GLfloat epsilon);

	/* glmReadPPM: read a PPM raw (type P6) file.  The PPM file has a header
	* that should look something like:
	*
	*    P6
	*    # comment
	*    width height max_value
	*    rgbrgbrgb...
	*
	* where "P6" is the magic cookie which identifies the file type and
	* should be the only characters on the first line followed by a
	* carriage return.  Any line starting with a # mark will be treated
	* as a comment and discarded.   After the magic cookie, three integer
	* values are expected: width, height of the image and the maximum
	* value for a pixel (max_value must be < 256 for PPM raw files).  The
	* data section consists of width*height rgb triplets (one byte each)
	* in binary format (i.e., such as that written with fwrite() or
	* equivalent).
	*
	* The rgb data is returned as an array of unsigned chars (packed
	* rgb).  The malloc()'d memory should be free()'d by the caller.  If
	* an error occurs, an error message is sent to stderr and NULL is
	* returned.
	*
	* filename   - name of the .ppm file.
	* width      - will contain the width of the image on return.
	* height     - will contain the height of the image on return.
	* 读取PPM格式的图形文件
	* 返回一个24位色的图象指针（可用于OPENGL纹理映射和图象绘制函数）
	* 同时图象尺寸存储在width，height指针参数指定的地址中
	*/
	GLubyte*
		glmReadPPM(const char*filename, int* width, int* height);
#ifdef __cplusplus
}
#endif
#endif
