
#include <stdlib.h>
#include <iostream>

#include "VertexData.h"
#include "PatternGenerator.h"


PatternGenerator::PatternGenerator()
{
}

PatternGenerator::~PatternGenerator()
{
}

void PatternGenerator::InitTiling()
{
    // 6.6.6
    AVector trans1(2.0000000000000004, -7.740128356567047E-16);
    AVector trans2(1.0, 1.7320508075688767);
    AVector centerPt(this->_img_width / 2, this->_img_height / 2);
    for(int a = 0; a < 10; a++)
    {
        for(int b = 0; b < 10; b++)
        {
            float sides = 6;
            float radAngle = (M_PI / (float)sides);
            float radius = 1.0 / cos(radAngle);
            std::vector<AVector> shape = GenerateNGon(sides, radius, 0, centerPt + trans1 * a + trans2 * b);
            ConcatNGon(shape, _tilingLines);
        }
    }


    /*
    // 4.8.8
    AVector trans1(2.8284271247461925, -5.455873044871822e-16);
    AVector trans2(1.414213562373095, -1.4142135623730971);
    AVector centerPt(this->_img_width / 2, this->_img_height / 2);
    const float matVals [9] = {0.41421356237309553, 1.9048415650791867e-17, 1.4142135623730954,
                               -1.9048415650791867e-17, 0.41421356237309553, -3.885780586188048e-16,
                               0, 0, 0};
    QMatrix3x3 mat(matVals);
    for(int a = 0; a < 1; a++)
    {
        for(int b = 0; b < 2; b++)
        {
            // rectangle
            float rSides = 4;
            float rRadAngle = (M_PI / (float)rSides);
            float rRadius = 1.0 / cos(rRadAngle);

            std::vector<AVector> rShape = GenerateNGon(rSides, rRadius, rRadAngle, AVector(0, 0));
            MultiplyShape(mat, rShape);
            AVector pos = centerPt + trans1 * a + trans2 * b;
            for(int i = 0; i < rShape.size(); i++)
            {
                rShape[i] += pos;
            }
            ConcatNGon(rShape, _tilingLines);

            // octagon
            float oSides = 8;
            float oRadAngle = (M_PI / (float)oSides);
            float oRadius = 1.0 / cos(oRadAngle);
            std::vector<AVector> oShape = GenerateNGon(oSides, oRadius, oRadAngle, centerPt + trans1 * a + trans2 * b);
            ConcatNGon(oShape, _tilingLines);
        }
    }
    */

    // 3.12.12
    /*
    AVector trans1(2.0000000000000018, -8.049116928532385E-16);
    AVector trans2(1.0000000000000004, 1.732050807568879);
    AVector centerPt(this->_img_width / 2, this->_img_height / 2);
    const float matVals1 [9] = {-1.3877787807814457E-17, 0.1547005383792514, 1.0811605818364125,
                                -0.1547005383792514, -1.3877787807814457E-17, -0.8208320146988646,
                               0, 0, 0};
    QMatrix3x3 mat1(matVals1);

    const float matVals2 [9] = {0.1339745962155616, 0.07735026918962591, 1.0811605818364132,
                                -0.07735026918962591, 0.1339745962155616, 0.3338685236803877,
                               0, 0, 0};
    QMatrix3x3 mat2(matVals2);

    const float matVals3 [9] = {1.0, 0.0, 0.08116058183641295,
                                0.0, 1.0, -0.2434817455092384,
                                0, 0, 0};
    QMatrix3x3 mat3(matVals3);

    for(int a = 0; a < 10; a++)
    {
        for(int b = 0; b < 10; b++)
        {
            // 2 triangles
            float tSides = 3;
            float tRadAngle = (M_PI / (float)tSides);
            float tRadius = 1.0 / cos(tRadAngle);

            std::vector<AVector> tShape1 = GenerateNGon(tSides, tRadius, tRadAngle / 2.0f, AVector(0, 0));
            MultiplyShape(mat1, tShape1);
            AVector pos1 = centerPt + trans1 * a + trans2 * b;
            for(int i = 0; i < tShape1.size(); i++)
                { tShape1[i] += pos1; }
            ConcatNGon(tShape1, _tilingLines);

            std::vector<AVector> tShape2 = GenerateNGon(tSides, tRadius, tRadAngle / 2.0f, AVector(0, 0));
            MultiplyShape(mat2, tShape2);
            AVector pos2 = centerPt + trans1 * a + trans2 * b;
            for(int i = 0; i < tShape2.size(); i++)
                { tShape2[i] += pos2; }
            ConcatNGon(tShape2, _tilingLines);


            // 12 -gon
            float twSides = 12;
            float twRadAngle = (M_PI / (float)twSides);
            float twRadius = 1.0 / cos(twRadAngle);
            std::vector<AVector> twShape = GenerateNGon(twSides, twRadius, twRadAngle, AVector(0, 0));
            MultiplyShape(mat3, twShape);
            AVector pos3 = centerPt + trans1 * a + trans2 * b;
            for(int i = 0; i < twShape.size(); i++)
            {
                twShape[i] += pos3;
            }
            ConcatNGon(twShape, _tilingLines);

        }
    }
    */
    PrepareLinesVAO(_tilingLines, &_tilingLinesVbo, &_tilingLinesVao, QVector3D(1.0, 0.0, 0.0));
}

void PatternGenerator::Paint()
{
    _shaderProgram->setUniformValue(_use_color_location, (GLfloat)1.0);

    if(_tilingLines.size() != 0)
    {
        glLineWidth(2.0f);
        _tilingLinesVao.bind();
        glDrawArrays(GL_LINES, 0, _tilingLines.size() * 2);
        _tilingLinesVao.release();
    }
}

void PatternGenerator::ConcatNGon(std::vector<AVector> sourcePolygon, std::vector<ALine> &destinationLines)
{
    for(uint a = 0; a < sourcePolygon.size(); a++)
    {
        destinationLines.push_back(ALine(sourcePolygon[a],
                                         sourcePolygon[(a + 1) % sourcePolygon.size()]));
    }
}

void PatternGenerator::MultiplyShape(QMatrix3x3 mat, std::vector<AVector>& shape)
{
    for(int a = 0; a < shape.size(); a++)
    {
        shape[a] = MultiplyVector(mat, shape[a]);
    }
}

AVector PatternGenerator::MultiplyVector(QMatrix3x3 mat, AVector vec)
{
    float x = mat(0, 0) * vec.x + mat(0, 1) * vec.y + mat(0, 2) * 1.0;
    float y = mat(1, 0) * vec.x + mat(1, 1) * vec.y + mat(1, 2) * 1.0;
    //float z = mat(2, 0) * vec.x + mat(2, 1) * vec.y + mat(2, 2) * 1.0;
    return AVector(x, y);
}

std::vector<AVector> PatternGenerator::GenerateNGon(float sides, float radius, float angleOffset, AVector centerPt)
{
    std::vector<AVector> shape;
    float addValue = (M_PI * 2.0 / (float)sides);
    for(float a = angleOffset; a < M_PI * 2.0 + angleOffset; a += addValue)
    {
        float xPt = centerPt.x + radius * sin(a);
        float yPt = centerPt.y + radius * cos(a);
        shape.push_back(AVector(xPt, yPt));
    }
    return shape;
}

void PatternGenerator::PrepareLinesVAO(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol)
{
    if(linesVao->isCreated())
    {
        linesVao->destroy();
    }

    linesVao->create();
    linesVao->bind();

    QVector<VertexData> data;
    for(uint a = 0; a < lines.size(); a++)
    {
        data.append(VertexData(QVector3D(lines[a].XA, lines[a].YA,  0), QVector2D(), vecCol));
        data.append(VertexData(QVector3D(lines[a].XB, lines[a].YB,  0), QVector2D(), vecCol));
    }

    linesVbo->create();
    linesVbo->bind();
    linesVbo->allocate(data.data(), data.size() * sizeof(VertexData));

    quintptr offset = 0;

    _shaderProgram->enableAttributeArray(_vertexLocation);
    _shaderProgram->setAttributeBuffer(_vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    offset += sizeof(QVector2D);

    _shaderProgram->enableAttributeArray(_colorLocation);
    _shaderProgram->setAttributeBuffer(_colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    linesVao->release();
}
