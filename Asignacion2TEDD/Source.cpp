#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

template <class dataType>
class Pila {
	private:
		class Nodo {
			public:
				Nodo* siguiente;
				dataType dato;
			
				Nodo(dataType key) {
					dato = key;
					siguiente = NULL;
				}
		};

		Nodo* tope;
		int cantElementos;
	
	public:
		//Constructor
		Pila() {
			tope = NULL;
			cantElementos = 0;
		}

		void push(dataType key);
		dataType pop();
		const dataType topeDePila();
		const bool pilaVacia();
		bool limpiarPila();
		int getCantElements();
		void imprimirElementos();

		//Destructor
		~Pila() {
			limpiarPila();
		}
};

template <class dataType>
const bool Pila<dataType>::pilaVacia() {
	if (tope == NULL) {
		return true;
	}

	return false;
}

template <class dataType>
bool Pila<dataType>::limpiarPila() {
	Nodo* aux;
	if (pilaVacia()) {
		return false;
	}

	while (!pilaVacia()) {
		aux = tope;
		tope = tope->siguiente;
		delete aux;
		cantElementos--;
	}
	return true;
}

template <class dataType>
void Pila<dataType>::push(dataType key) {
	Nodo* nuevoElemento;

	nuevoElemento = new Nodo(key);
	nuevoElemento->siguiente = tope;
	tope = nuevoElemento;

	cantElementos++;
}

template <class dataType>
dataType Pila<dataType>::pop() {
	if (pilaVacia()) {
		cerr << "ERROR: UNDERFLOW - Pila vacia, no se puede extraer el elemento." << endl;
		throw std::invalid_argument("Underflow");
	}

	dataType aux = tope->dato;
	tope = tope->siguiente;

	cantElementos--;
	return aux;
}

template <class dataType>
const dataType Pila<dataType>::topeDePila() {
	if (pilaVacia()) {
		cerr << "ERROR: UNDERFLOW - Pila vacia, no se puede leer el elemento en el tope de la pila" << endl;
		return NULL;
	}

	return tope->dato;
}

template <class dataType>
int Pila<dataType>::getCantElements() {
	return cantElementos;
}

template <class dataType>
void Pila<dataType>::imprimirElementos() {
	if (pilaVacia()) {
		cerr << "Error: La pila se encuentra vacia." << endl;
	}

	Nodo* aux = tope;
	int auxCant = cantElementos;

	while (aux != NULL) {
		cout << "Elemento " << auxCant << " : " << aux->dato << endl;
		auxCant--;
		aux = aux->siguiente;
	}
}

struct Producto {
	int id = 0;
	string nombre = "";
	float precio = 0;
	int cantidad = 0;
};

class Inventario {
	private:
		fstream inventario;
	public:
		void agregarProducto(Producto p);
		void buscarRegistro(Producto &prod, long registro);
		int nRegistros(Producto &prod);
		void imprimirInventario();
		void ventaProducto();
		void modificarPrecio();
};

void Inventario::agregarProducto(Producto p) {
	inventario.open("inventario.bin", ios::out | ios::binary | ios::app);
	if (!inventario) {
		cerr << "Ha ocurrido un error al intentar abrir el archivo para agregar un producto" << endl;
		return;
	}
	inventario.write(reinterpret_cast<char*> (&p), sizeof(Producto));
	inventario.close();
}

void Inventario::buscarRegistro(Producto &prod, long registro) {
	inventario.open("inventario.bin", ios::binary | ios::in);
	if (!inventario) {
		cerr << "Error al intentar abrir el archivo para lectura." << endl;
		return;
	}
	long desplazamiento = (long)sizeof(Producto) * registro;
	inventario.seekg(desplazamiento, ios::beg);
	inventario.read(reinterpret_cast<char*> (&prod), sizeof(Producto));
	inventario.close();
}

int Inventario::nRegistros(Producto &prod) {
	inventario.open("inventario.bin", ios::binary | ios::in);
	if (!inventario) {
		cerr << "Error al intentar contar el numero de registros disponibles" << endl;
		return NULL;
	}
	int n = 0;

	while (inventario.read((char*)&prod, sizeof(prod))) {
		n++;
	}
	inventario.close();
	return n;
}

void Inventario::imprimirInventario() {
	Producto *prod;
	prod = new Producto;
	int nReg = nRegistros(*prod);

	cout << "ID" << "\t" << "NOMBRE" << "\t" << "PRECIO" << "\t" << "CANTIDAD" << endl;
	for (long i = 0; i < nReg; i++) {
		buscarRegistro(*prod, i);
		cout << prod->id << "\t" << prod->nombre << "\t" << prod->precio << "\t" << prod->cantidad << endl;
	}

	prod = NULL;
	delete prod;
}

void Inventario::modificarPrecio() {
	int id;
	int changeto;
	Producto* prod;
	prod = new Producto;
	long cantidadDeStock = nRegistros(*prod);
	Pila<Producto> pilaInicial, pilaFinal;
	imprimirInventario();

	cout << "Teclee el ID del producto al cual desea modificar el precio: ";
	cin >> id;
	cout << endl;

	cout << "Teclee el nuevo precio del producto: ";
	cin >> changeto;
	cout << endl;

	for (long i = 0; i < cantidadDeStock; i++) {
		buscarRegistro(*prod, i);
		if (prod->id == id) {
			prod->precio = changeto;
			pilaInicial.push(*prod);
		}
		else {
			pilaInicial.push(*prod);
		}
	}

	for (long i = 0; i < cantidadDeStock; i++) {
		pilaFinal.push(pilaInicial.pop());
	}

	inventario.open("inventario.bin", ios::out | ios::trunc);
	inventario.close();

	for (long i = 0; i < cantidadDeStock; i++) {
		agregarProducto(pilaFinal.pop());
	}

	cout << "Inventario modificado. El nuevo inventario es: " << endl;

	imprimirInventario();

}

void Inventario::ventaProducto() {
	
	int id;
	int changeto;
	Producto* prod;
	prod = new Producto;
	long cantidadDeStock = nRegistros(*prod);
	Pila<Producto> pilaInicial, pilaFinal;
	imprimirInventario();

	cout << "Teclee el ID del producto que desea vender: ";
	cin >> id;
	cout << endl;

	cout << "Teclee cuantas unidades desea vender: ";
	cin >> changeto;
	cout << endl;

	for (long i = 0; i < cantidadDeStock; i++) {
		buscarRegistro(*prod, i);
		if (prod->id == id) {
			if (prod->cantidad >= changeto) {
				prod->cantidad -= changeto;
			}
			else {
				cerr << "Error: No puede vender mas productos de los que tiene en stock." << endl;
				return;
			}
			pilaInicial.push(*prod);
		}
		else {
			pilaInicial.push(*prod);
		}
	}

	for (long i = 0; i < cantidadDeStock; i++) {
		pilaFinal.push(pilaInicial.pop());
	}

	inventario.open("inventario.bin", ios::out | ios::trunc);
	inventario.close();

	for (long i = 0; i < cantidadDeStock; i++) {
		agregarProducto(pilaFinal.pop());
	}

	cout << "Inventario modificado. El nuevo inventario es: " << endl;

	imprimirInventario();
}


void guardaFechayHora(string mensaje) {
	fstream log("log.txt", ios::out | ios::app);
	if (!log) {
		cerr << "Error al intentar guardar la hora y lafecha" << endl;
		return;
	}
	char currDate[50];

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(currDate, "%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	log << mensaje << "		/ Fecha y Hora: " << currDate << " /" << endl;
}

int main(int args, const char* argsv[]) {
	Inventario invent;
	return 0;
}