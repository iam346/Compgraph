#ifndef _2DPOINT_
#define _2DPOINT_

namespace graph
{
template <class Type> class point2D
{

private:
    Type x,y;

public:
    point2D ( Type xk=0, Type yk=0 ) :x ( xk ),y ( yk ) {};

    point2D ( const point2D<Type>& a ) {
        this->x = a.x;
        this->y = a.y;
    }

    Type getX() const {
        return this->x;
    }

    Type getY() const {
        return this->y;
    }

    void setPoint ( Type nx,Type ny ) {
        this->x=nx;
        this->y=ny;
    }

    void setPoint ( const point2D<Type> &point ) {
        this->x = point.getX();
        this->y = point.getY();
    }

    void setX ( Type x ) {
        this->x = x;
    }

    void setY ( Type y ) {
        this->y = y;
    }

    bool operator== ( const point2D<Type> other ) {
        if ( other.getX() == x && other.getY() == y ) {
            return true;
        }
        return false;
    }

    Type dist2 ( const point2D<Type>& a ) const {
        Type t1 = this->x - a.getX();
        Type t2 = this->y - a.getY();

        return t1 * t1 + t2 * t2;
    }

    Type getSize() {
        return sqrt ( x*x + y*y );
    }

    point2D<Type>& operator= ( const point2D<Type>& a ) {
        x = a.getX();
        y = a.getY();

        return *this;
    }

    Type operator* ( const point2D<Type>& other ) {
        Type temp = other.getX() *x +other.getY() *y;
        return temp;
    }

    point2D<Type> operator* ( Type scalar ) {
        point2D<Type> temp;
        temp.setX ( getX() *scalar );
        temp.setY ( getY() *scalar );
        return temp;
    }

    point2D<Type> operator- ( point2D<Type> other ) {
        point2D<Type> temp;
        temp.setX ( getX()-other.getX() );
        temp.setY ( getY()-other.getY() );

        return temp;
    }

    point2D<Type> operator+ ( point2D<Type> other ) {
        point2D<Type> temp;
        temp.setX ( getX() + other.getX() );
        temp.setY ( getY() + other.getY() );

        return temp;
    }
};
}

#endif
