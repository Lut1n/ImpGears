#ifndef IMP_MAT4_H
#define IMP_MAT4_H


class Mat4
{
    public:
        Mat4();
        Mat4(const Mat4& other);
        Mat4(const float* data);
        virtual ~Mat4();

        const Mat4& operator=(const Mat4& other);

        void setData(const float* data);
        const float* getData() const;

    protected:
    private:

        float m_data[16];
};

#endif // IMP_MAT4_H
