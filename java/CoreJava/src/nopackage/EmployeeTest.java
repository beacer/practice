package testpackage;
import java.util.Date;
import java.util.GregorianCalendar;

public class EmployeeTest {
    public static void main(String[] args) {
        Employee[] staff = new Employee[3];

        staff[0] = new Employee("Carl Cracker", 75000, 1987, 12, 15);
        staff[1] = new Employee("Harry Hacker", 50000, 1989, 10, 1);
        staff[2] = new Employee("Tony Tester", 40000, 1990, 3, 15);

        for (Employee e : staff) {
            e.raiseSalary(5);
        }

        for (Employee e: staff) {
            System.out.printf("name=%s, salary=%.2f, hireday=%s\n",
                    e.getName(), e.getSalary(), e.getHireDay());
        }

    }

}

/**
 *
 */
class Employee {
    private String name;
    private double salary;
    private Date hireDay;

    public Employee(String n, double s, int year, int month, int day) {
        name = n;
        salary = s;
        GregorianCalendar calendar = new GregorianCalendar(year, month - 1, day);
        hireDay = calendar.getTime();
    }

    public String getName() {
        return name;
    }

    public double getSalary() {
        return salary;
    }

    public String getHireDay() {
        return hireDay.toString();
    }

    public void raiseSalary(double percent) {
        salary += salary * percent / 100;
    }
}
