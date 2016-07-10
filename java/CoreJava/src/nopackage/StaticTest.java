import org.omg.PortableInterceptor.SYSTEM_EXCEPTION;

/**
 *
 */
public class StaticTest {
    public static void main(String[] args) {
        Employee[] staff = new Employee[3];

        staff[0] = new Employee("Tom", 40000);
        staff[1] = new Employee("Dick", 60000);
        staff[2] = new Employee("Harry", 65000);

        for (Employee e: staff) {
            e.setId();
            System.out.println("name=" + e.getName() + ", id = " + e.getId() +
                               ", salary = " + e.getSalary());
        }

        int n = Employee.getNextId();
        System.out.println("Next available id=" + n);
    }
}

class Employee {
    private static int nextId = 1;

    private String name;
    private double salary;
    private int id;

    public Employee(String name, double salary) {
        this.name = name;
        this.salary = salary;
        id = 0;
    }

    public String getName() {
        return this.name;
    }

    public double getSalary() {
        return this.salary;
    }

    public int getId() {
        return this.id;
    }

    public void setId() {
        this.id = Employee.nextId++;
    }

    public static int getNextId() {
        return Employee.nextId;
    }
}
