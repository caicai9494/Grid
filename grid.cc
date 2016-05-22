#include <iostream>
#include <string>

// ----------------------------TYPELIST-------------------------------------

class TypeNull {};

template <typename HEAD, typename TAIL>
struct TypeList {
    typedef HEAD Head;
    typedef TAIL Tail;
};

#define TYPELIST_1(T1)  TypeList<T1, TypeNull>
#define TYPELIST_2(T1, T2)  TypeList<T1, TYPELIST_1(T2) >
#define TYPELIST_3(T1, T2, T3)  TypeList<T1, TYPELIST_2(T2, T3) >

enum ElemType {
    INT,
    FLOAT,
    STRING,
    DATE
};

// ----------------------------COLUMN-------------------------------------
template <size_t N, typename T>
struct Column {
    typedef T ValueType; 
    size_t size() const { return N; }

    std::string d_name;
    ElemType d_type;
    ValueType d_value[N];
};

// ----------------------------GRID-------------------------------------
template <size_t N, typename TL>
struct Grid {
    typedef typename TL::Head ValueType; 
    typedef typename TL::Tail TypeList; 
    typedef Grid<N, TypeList> SubGrid;
    typedef Column<N, ValueType> ColumnType; 

    ColumnType d_column; 
    SubGrid d_grid;
};

template <size_t N>
struct Grid<N, TypeNull> {
};

#define GRID_1(T1)  Grid<1, TYPELIST_1(T1)>
#define GRID_2(T1, T2)  Grid<2, TYPELIST_2(T1, T2)>
#define GRID_3(T1, T2, T3)  Grid<3, TYPELIST_3(T1, T2, T3)>

// ----------------------------GRID GET COLUMN NUMBER-------------------

template <typename T> 
struct ColumnLength;

template <template 
           <size_t, typename> class Grid, size_t N 
         >
struct ColumnLength <Grid<N, TypeNull> > {
    enum { value = 0 };
};

template <template 
           <size_t, typename> class Grid, size_t N, typename TL 
         >
struct ColumnLength <Grid<N, TL> >  {
    enum { value = 1 + ColumnLength<Grid<N, typename TL::Tail> >::value };
};

// ----------------------------GRID PRINTER-------------------------------------
template <typename T> 
struct Printer;

template <template 
           <size_t, typename> class Grid, size_t N 
         >
struct Printer <Grid<N, TypeNull> > {
    void print(std::ostream& stream, const Grid<N, TypeNull>& grid) {} 
};

template <template 
           <size_t, typename> class Grid, size_t N, typename TL 
         >
struct Printer <Grid<N, TL> >  {
    void print(std::ostream& stream, const Grid<N, TL>& grid) 
    {
	typedef typename Grid<N, TL>::ColumnType ColumnType;
	typedef typename Grid<N, TL>::SubGrid SubGrid;
	const ColumnType &col = grid.d_column;
	stream << "column name: " << col.d_name << '\n';
	stream << "column data: [   ";
	for (size_t i = 0; i != N; ++i) {
	    stream << col.d_value[i] << "  ";
	}
	stream << "]\n";

	Printer<SubGrid> printer;
	printer.print(stream, grid.d_grid);
    } 
};

int main()
{
    using namespace std;

    typedef GRID_3(int, float, std::string) MyGrid;

    cout << "Grid Row Number: ";
    cout << ColumnLength<MyGrid>::value << '\n';
    MyGrid grid = {
	"number",
	INT,
	{1, 2, 3},
	    {
		"price",
		FLOAT,
		{1.1, 2.2, 3.3},
		{
		    {
			"ticker",
			STRING,
			{"a", "b", "c"}
		    }
		}
	    }
    };

    Printer<MyGrid> printer;
    printer.print(std::cout, grid);
    return 0;
}
